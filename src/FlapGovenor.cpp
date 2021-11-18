#include "FlapGovenor.h"

#include <avr/interrupt.h>
#include <avr/io.h>

namespace FlapGovenor{

ISR(TIMER0_COMPA_vect){
    PINB |= _BV(PIN7);
}

void init(){
    // Reset timer defaults
    OCR0B = 0;
    TCCR0A = _BV(WGM01);
    // prescaler /1024 
    TCCR0B = _BV(CS02) | _BV(CS00);
    // Count to 117 for 100Hz @ 12MHz F_CPU
    OCR0A = 117;
    TIMSK0 = 0;
    
    DDRB = _BV(PIN7);
}

void enable(){
    TIMSK0 |= _BV(OCIE0A);
}
void disable(){
    TIMSK0 &= ~_BV(OCIE0A);
}

}
