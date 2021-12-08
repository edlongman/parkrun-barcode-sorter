#include <avr/io.h>

#include "unittest_transport.h"

uint16_t baud = 57600;

void unittest_uart_begin() {
    UBRR0 = F_CPU/16/baud-1;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

void unittest_uart_putchar(char ch) {
    // Wait for ready then send
    while ( !( UCSR0A & _BV(UDRE0)) )
    ;
    UDR0 = ch;
}

void unittest_uart_flush() {
    // Wait for ready then send
    while ( !( UCSR0A & _BV(UDRE0)) )
    ;
}

void unittest_uart_end() {

}