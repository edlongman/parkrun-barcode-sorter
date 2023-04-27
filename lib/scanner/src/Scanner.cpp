#include <Scanner.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <algorithm>
#include <math.h>

namespace std{
uint8_t min(uint8_t x, uint8_t y){
    return std::min((unsigned int)x, (unsigned int)y);
}
}

namespace Scanner{

uint8_t rx_scan_limit = 0;
char* rx_scan_buffer = nullptr;
uint8_t* command_response_buffer;

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
    unsigned int baud_speed;
} CommsState;

// 32 byte TX buffer
constexpr uint8_t payload_length{26};
constexpr uint8_t command_header_length{6};
constexpr uint8_t crc_length{2};
typedef struct {
    uint8_t position{0};
    uint8_t limit{0};
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
    else if(usart_get_flag(USART1, USART_SR_TXE)){
        usart_send(USART1, tx_buffer.data_union.buffer[tx_buffer.position++]);
        if(tx_buffer.position >= tx_buffer.limit){
            usart_disable_tx_interrupt(USART1);
            if(comms_state.mode == CommandMode::SPEED_CHANGE){
                comms_state.mode = CommandMode::COMMAND;
                // USART_CR1(USART1) |= USART_CR1_TCIE;
            }
        }
    }
    else if(usart_get_flag(USART1, USART_FLAG_TC)){
        if(comms_state.mode == CommandMode::SPEED_CHANGE){
            // usart_disable(USART1);
            // Disable frame complete interrupt
            // USART_CR1(USART1) &= ~USART_CR1_TCIE;
            // ?? Clear frame complete flag (not implemented)
            // USART_SR(USART1) &= ~USART_SR_TC;
            //usart_set_baudrate(USART1, comms_state.baud_speed);
            comms_state.mode = CommandMode::COMMAND;
            // usart_enable(USART1);
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

void changeSpeed(unsigned int baud, uint8_t* data_response){
    // Swapped address bytes for little endian
    uint16_t baud_address = 0x2A00;
    constexpr uint8_t baud_reg_size = 2;
    uint8_t* send_data;
    uint8_t reg57k[baud_reg_size] = {0x34, 0x00}; // 57600 Baud
    uint8_t reg9k6[baud_reg_size] = {0x39, 0x01}; // 9600 Baud
    switch(baud){
        case 57600:
            send_data = reg57k;
            break;
        default: 
        case 9600: 
            baud = 9600;
            send_data = reg9k6;
    }
    Scanner::sendCommandForResponse(Scanner::CommandType::WRITE, baud_address, 2, send_data, data_response);
    comms_state.baud_speed = baud;
    comms_state.mode = CommandMode::SPEED_CHANGE;
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

void prepareCommand(const command_type type, const uint16_t address, const uint8_t length, const uint8_t* send, uint8_t* response){
    auto tx_fields = &tx_buffer.data_union.fields;
    // Swapped order for little endian
    tx_fields->head = 0x007E;
    tx_fields->type = type;
    tx_fields->length = length;
    tx_fields->address = address;
    uint8_t copy_limit = std::min((unsigned int)payload_length - crc_length, (unsigned int)length);
    auto crc_field = std::copy_n(send, copy_limit, tx_fields->payload);
    // Do not verify special value
    crc_field[0] = 0xAB;
    crc_field[1] = 0xCD;
    if(type == CommandType::SAVE && address == 0x00){
        if(send[0] == 0x00){
            // Save CRC Special Value
            crc_field[0] = 0xDE;
            crc_field[1] = 0xC8;
        }
        else if(send[0] == 0xFF){
            // Factory Reset Special Value
            crc_field[0] = 0x00; // ?? unknown
            crc_field[1] = 0x00; // ?? unknown
        }
    }

    // Set tx tracking points and start transmission
    tx_buffer.position = 0;
    tx_buffer.limit = std::min(command_header_length + length + crc_length, command_header_length + payload_length);
}

void sendCommandForResponse(const command_type type, const uint16_t address, const uint8_t length, const uint8_t* send, uint8_t* response){
    prepareCommand(type, address, length, send, response);

    comms_state.mode = CommandMode::COMMAND;
    rx_buffer.position = 0;
    usart_send(USART1, tx_buffer.data_union.buffer[tx_buffer.position++]);
    usart_enable_tx_interrupt(USART1);
    // Don't care about command, fill with a value for now
    command_response_buffer = response;
    command_response_buffer[0] = 0x55;
    //command_response_buffer[1] = 0x54;
}

bool isCommandTxComplete(){
    // Check for valid lengths
    const auto tx_fields = tx_buffer.data_union.fields;
    return (tx_fields.length > 0 && tx_fields.length<payload_length-crc_length
                && tx_buffer.position >= command_header_length + tx_fields.length + crc_length);
}

bool isCommandRxComplete(){
    const auto rx_fields = rx_buffer.data_union.fields;
    unsigned int declared_length = rx_fields.length;
    if(declared_length == 0)declared_length = 256;
    uint8_t buffer_limit = std::min(response_header_length + rx_fields.length + crc_length, rx_length_limit);
    if (comms_state.mode == CommandMode::COMMAND && rx_buffer.position >= buffer_limit){
        if(command_response_buffer!=nullptr){
            std::copy_n(rx_buffer.data_union.fields.payload, buffer_limit, command_response_buffer);
        }
        comms_state.mode = CommandMode::OFF;
        return true;
    }
    return false;
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