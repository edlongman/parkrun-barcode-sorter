#include <stdint.h>
#include <ServoController.h>
#include <avr/io.h>
#include <avr/interrupt.h>

namespace ServoController{

// Calculate the timer top value using floating point arithmetic and rounding
#define TIMER_TOP(FREQ, PRESCALE) (float)(F_CPU)/PRESCALE/FREQ-0.5

const uint16_t timer_frequency = 40;
const uint8_t prescaler = 8;
const uint16_t timer_top = TIMER_TOP(timer_frequency, prescaler);

#define PWM_CALC(proportion) (uint32_t)timer_top*proportion/UINT16_MAX;

void init(){
    // Initialize servo timer
    // 9 bit Fast PWM mode
    TCCR1A = _BV(WGM11) | _BV(WGM10);
    // Prescaler of 1024
    TCCR1B =  _BV(WGM12) | _BV(CS12);

    OCR1B = 0;

    DDRD |= _BV(PD4);
    PORTD &= ~_BV(PD4);
    DDRB |= _BV(7);
}

void enable(){
    TCCR1A |= _BV(COM1B1);
}

void disable(){
    TCCR1A &= ~_BV(COM1B1);
}

void setPWM(uint16_t val){
    uint8_t sreg;
    sreg = SREG;
    cli();
    if(val<70){
        val = 0;
    }
    else if(val<80){
        val = 80;
    }
    else if(val>110){
        val = 110;
    }
    OCR1B = val;
    SREG = sreg;
}

}