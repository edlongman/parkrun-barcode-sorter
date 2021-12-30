#include <util/delay.h>
#include <avr/interrupt.h>
#include <ConsoleUart.h>
#include <stdio.h>

ISR(TIMER1_OVF_vect){
    PINB |= _BV(PB7);
}

void test_servo_pos(uint8_t val, char* tx_buffer, uint8_t length){
    OCR1B = val;
    sprintf(tx_buffer, "%3u\r\n", val);
    ConsoleUart::blockingSendLine(tx_buffer, length);
}

int main(){
    sei();
    TIMSK1 = _BV(TOIE1);

    
    // Initialize servo timer
    // 9 bit Fast PWM mode
    TCCR1A = _BV(WGM11) | _BV(WGM10);
    // Prescaler of 1024
    TCCR1B =  _BV(WGM12) | _BV(CS12);

    OCR1B = 0;

    DDRD |= _BV(PD4);
    PORTD &= ~_BV(PD4);
    DDRB |= _BV(7);
    _delay_ms(2000);
    TCCR1A |= _BV(COM1B1);

    const uint8_t buff_size = 20;
    char tx_buffer[buff_size];

    uint8_t center_estimate = 97;
    for(uint8_t i = 5; i<50;i++){
        test_servo_pos(center_estimate - i, tx_buffer, buff_size);
        _delay_ms(2000);
        test_servo_pos(center_estimate + i, tx_buffer, buff_size);
        _delay_ms(2000);
    }
    TCCR1A &= ~_BV(COM1B1);
    
    while(1);
}