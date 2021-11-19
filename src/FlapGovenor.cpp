#include "FlapGovenor.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "Util.h"

namespace FlapGovenor{

const uint16_t timer_frequency = 100;

ISR(TIMER0_COMPA_vect){
    PINB |= _BV(PIN7);
}

void init(){
    // Reset timer defaults
    OCR0B = 0;
    TCCR0A = _BV(WGM01);
    // prescaler /1024 
    const uint16_t prescaler = 1024;
    TCCR0B = _BV(CS02) | _BV(CS00);
    // Count to 116 for 100Hz @ 12MHz F_CPU (117-1)
    OCR0A = TIMER_TOP(timer_frequency, prescaler);
    TIMSK0 = 0;
    
    DDRC = 0xFF;
    DDRB = _BV(PIN7);
}

void enable(){
    TIMSK0 |= _BV(OCIE0A);
}
void disable(){
    TIMSK0 &= ~_BV(OCIE0A);
}

}
