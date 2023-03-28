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
    usart_set_mode(USART1, USART_MODE_TX_RX);
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