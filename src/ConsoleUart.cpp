#include <avr/io.h>
#include <avr/interrupt.h>
#include "ConsoleUart.h"

namespace ConsoleUart{

char* rx_scan_buffer = nullptr;
uint8_t rx_scan_position = 0;
uint8_t rx_scan_limit = 0;
bool rx_end_received = true;
char last_received = 0;

ISR(USART0_RX_vect)
{
    // Read byte regardless of buffer state
    last_received = UDR0;
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
    UBRR0 = F_CPU/16/baud-1;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

void enable(){
    UCSR0B |= _BV(RXCIE0);
}

void disable(){
    UCSR0B &= ~_BV(RXCIE0);
}

void blockingSendChar(const char ch){
    // Wait for ready then send
    while ( !( UCSR0A & _BV(UDRE0)) )
    ;
    UDR0 = ch;
}

void blockingSendLine(const char* buffer, const uint8_t length){
    uint8_t tx_position = 0;
    // Loop through input buffer until end null char
    while(tx_position < length){
        switch(buffer[tx_position]){
            case '\r':
            case '\n':
            case '\0':
                goto exit_loop;
        }
        blockingSendChar(buffer[tx_position]);
        tx_position++;
    }
    exit_loop: ;
    // Send line terminators
    blockingSendChar('\r');
    blockingSendChar('\n');
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