#include <Scanner.h>
#include <avr/interrupt.h>

namespace Scanner{

char* rx_scan_buffer = nullptr;
uint8_t rx_scan_position = 0;
uint8_t rx_scan_limit = 0;
bool rx_end_received = true;
char last_received = 0;

const uint8_t nResetPin = 0;

ISR(USART1_RX_vect){
    // Read byte regardless of buffer state
    last_received = UDR1;
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

void init(){
    PORTC |= _BV(nResetPin) | _BV(triggerPin); // Enable Reset Pull-up and set trigger to default high
    DDRC |= _BV(triggerPin);

    UBRR1 = F_CPU/16/baud-1;
    UCSR1B = _BV(RXEN1) | _BV(TXEN1);

}

void enable(){
    UCSR1B |= _BV(RXCIE1);
}

void disable(){
    UCSR1B &= ~_BV(RXCIE1);
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