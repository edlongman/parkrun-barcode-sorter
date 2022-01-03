#include "FlapGovenor.h"
#include <avr/interrupt.h>
#include <avr/io.h>

namespace FlapGovenor{

const uint16_t timer_frequency = 100;
const uint8_t flap_count = 3;
const uint8_t map[flap_count]={PC6, PC4, PC5};
const uint8_t flap_control_mask = _BV(map[0]) | _BV(map[1]) | _BV(map[2]);

ISR(TIMER0_COMPA_vect){
    // Flap duty cycle limit logic
}

void init(){
    // Reset timer defaults
    OCR0B = 0;
    TCCR0A = _BV(WGM01);
    // prescaler /1024 
    const uint16_t prescaler = 1024;
    TCCR0B = _BV(CS02) | _BV(CS00);
    // Count to 116 for 100Hz @ 12MHz F_CPU (117-1)
    // OCR0A = TIMER_TOP(timer_frequency, prescaler);
    TIMSK0 = 0; //_BV(OCIE0A); 
    
    // Disable JTAG if using PORTC Outputs
    MCUCR |= _BV(JTD);
    MCUCR |= _BV(JTD); // Double write within 4 clock cycles
    DDRC |= flap_control_mask;
    PORTC &= ~flap_control_mask;
}

void enable(){
    TIMSK0 |= _BV(OCIE0A);
}
void disable(){
    TIMSK0 &= ~_BV(OCIE0A);
}

void setOn(const uint8_t index){
    if(index<flap_count){
        PORTC |= _BV(map[index]);
    }
}

void setOff(const uint8_t index){
    if(index<flap_count){
        PORTC &= (uint8_t)~_BV(map[index]);
    }
}

}
