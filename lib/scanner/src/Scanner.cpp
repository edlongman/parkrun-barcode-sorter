#include <Scanner.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <algorithm>
#include <math.h>

namespace Scanner{

uint8_t rx_scan_limit = 0;
char* rx_scan_buffer = nullptr;

namespace CommandMode {
    typedef enum {
        OFF,
        SCAN,
        SCAN_COMPLETE,
        COMMAND,
        SPEED_CHANGE
    } Num;
}

typedef struct {
    CommandMode::Num mode;
    bool high_speed;
} CommsState;

namespace CommandType{
    typedef enum {
        SUCCESS = 0x00,
        READ = 0x07,   
        WRITE = 0x08,
        FACTORY_RST = 0x09
    } command_type_num;
}

typedef uint8_t command_type;

// 32 byte TX buffer
constexpr uint8_t payload_length{26};
constexpr uint8_t command_header_length{6};
typedef struct {
    uint8_t position{0};
    union {
        char buffer[payload_length+command_header_length];
        struct {
            uint16_t head;
            command_type type;
            uint8_t length;
            uint16_t address;
            uint8_t payload[payload_length];
        } fields;
    } data_union;
} command_t;

// 30 byte RX buffer
constexpr uint8_t response_header_length{4};
constexpr uint8_t rx_length_limit = payload_length+response_header_length;
typedef struct {
    uint8_t position{0};
    union {
        char buffer[rx_length_limit];
        struct {
            uint16_t head;
            command_type type;
            uint8_t length;
            uint8_t payload[payload_length];
        } fields;
    } data_union;
} response_t;

response_t rx_buffer;
command_t tx_buffer;
CommsState comms_state;

// Command Mode

// SendCommand
// GetResponse

// Scan Mode

const unsigned int nResetPin = GPIO13;

}

void usart1_isr(){
    using namespace Scanner;
    // Read byte regardless of buffer state
    if (usart_get_flag(USART1, USART_SR_RXNE)) {
        const char last_received = usart_recv(USART1);
        if(rx_buffer.position < rx_length_limit && 
                (comms_state.mode == CommandMode::COMMAND || comms_state.mode == CommandMode::SCAN) ){
            rx_buffer.data_union.buffer[rx_buffer.position] = last_received;
            if(comms_state.mode == CommandMode::SCAN){
                // Put null byte at end of char array to symbolize string
                switch(last_received){
                    case '\r':
                    case '\n':
                    case '\0':
                        //gpio_clear(GPIOC, GPIO13);
                        comms_state.mode = CommandMode::SCAN_COMPLETE;
                        rx_buffer.data_union.buffer[rx_buffer.position] = '\0';
                        break;
                    default:
                        break;
                }
            }
            rx_buffer.position++;
        }
    }
}

namespace Scanner{

void init(unsigned int baud){
    gpio_set(GPIOB, triggerPin); // Enable Reset Pull-up and set trigger to default high
    gpio_clear(GPIOB, nResetPin);
	rcc_periph_clock_enable(RCC_GPIOB);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, triggerPin);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, nResetPin);
    rcc_periph_clock_enable(RCC_USART1);
	nvic_enable_irq(NVIC_USART1_IRQ);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_USART1_RX);
    gpio_set(GPIOA, GPIO8);

    usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_set_baudrate(USART1, baud);
    usart_enable(USART1);

}

void commsEnable(){
    usart_enable_rx_interrupt(USART1);
    //usart_enable_tx_interrupt(USART1);
}

void commsDisable(){
    usart_disable_rx_interrupt(USART1);
    //usart_disable_tx_interrupt(USART1);
}

void enable(){
    gpio_set(GPIOB, nResetPin);
    commsEnable();
}

void disable(){
    commsDisable();
}
void resetRxBuffer(){
    rx_buffer.position = 0;
    std::fill_n(rx_buffer.data_union.buffer, rx_length_limit, 0);
}

void startScan(){
    comms_state.mode = CommandMode::SCAN;
    gpio_clear(GPIOB, triggerPin);
}

void endScan(){
    gpio_set(GPIOB, triggerPin);
    comms_state.mode = CommandMode::OFF;
}

void scanLine(char* buffer, uint8_t length){
    resetRxBuffer();
    rx_scan_buffer = buffer;
    rx_scan_limit = length;
    
}

bool isScanComplete(){
    uint8_t copy_limit = std::min(rx_scan_limit, rx_length_limit);
    if(comms_state.mode == CommandMode::SCAN_COMPLETE ||
            (comms_state.mode == CommandMode::SCAN && rx_buffer.position == rx_scan_limit-1) ){
        if(rx_scan_buffer != nullptr)
            std::copy_n(rx_buffer.data_union.buffer, copy_limit, rx_scan_buffer);
        return true;
    }
    return false;
}

}