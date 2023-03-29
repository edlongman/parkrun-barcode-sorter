#include <Scanner.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>

namespace Scanner{

uint8_t rx_scan_position = 0;
uint8_t rx_scan_limit = 0;
bool rx_end_received = true;
char last_received = 0;
char* rx_scan_buffer = nullptr;

const unsigned int nResetPin = GPIO13;

}

void usart1_isr(){
	gpio_clear(GPIOC, GPIO13);

    using namespace Scanner;
    // Read byte regardless of buffer state
    last_received = usart_recv(USART1);
    if(rx_scan_buffer != nullptr && rx_scan_position < rx_scan_limit && not rx_end_received){
        rx_scan_buffer[rx_scan_position] = last_received;
        rx_scan_position++;
        // Put null byte at end of char array to symbolize string
        switch(last_received){
            case '\r':
            case '\n':
            case '\0':
                rx_end_received = true;
                rx_scan_buffer[rx_scan_position] = '\0';
        }
    }
}

namespace Scanner{

void init(){
    gpio_set(GPIOB, triggerPin); // Enable Reset Pull-up and set trigger to default high
    gpio_clear(GPIOB, nResetPin);
	rcc_periph_clock_enable(RCC_GPIOB);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, triggerPin);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, nResetPin);
    rcc_periph_clock_enable(RCC_USART1);
	nvic_enable_irq(NVIC_USART1_IRQ);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO8);
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
}

void commsDisable(){
    usart_disable_rx_interrupt(USART1);
}

void enable(){
    gpio_set(GPIOB, nResetPin);
    commsEnable();
}

void disable(){
    commsDisable();
}

void scanLine(char* buffer, uint8_t length){
    rx_scan_buffer = buffer;
    rx_scan_limit = length - 1;
    rx_scan_position = 0;
    rx_end_received = false;
}

bool isScanComplete(){
    return rx_end_received || rx_scan_position == rx_scan_limit;
}

}