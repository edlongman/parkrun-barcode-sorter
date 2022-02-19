#include "WheelGauge.h"
#include <avr/io.h>
#include <avr/interrupt.h>

namespace WheelGauge{

const uint8_t channel_mask = _BV(PD7) | _BV(PD5);

volatile int16_t motor_position = 0; // in edge counts
volatile uint8_t encoder_state = 0;

// Switch derived from encoder logic state table
// Very quick, only recording original pulses with no processing
ISR(PCINT3_vect){
    encoder_state = ((encoder_state & _BV(5)) >> 1) | (PIND & _BV(5));
    switch(encoder_state >> 4){
        case 0b0010:
            motor_position++;
            motor_position++;
            break;
        case 0b0001:
            motor_position++;
            motor_position++;
            break;
        default:
            //nothing
            break;
    }
}

void init(){
    // set pins 5 and 7 as inputs with pull-ups
    DDRD &= ~channel_mask;
    PORTD |= channel_mask;
    // Setup pins 5 and 7 change interrupt mask
    PCMSK3 |= channel_mask;
}

void enable(){
    PCICR = _BV(PCIE3);
}

void disable(){
    PCICR = _BV(PCIE3);
}

void teardown(){
    // Must disable output before changing data direction
    PORTD &= ~channel_mask;
    DDRD |= channel_mask;
    PCMSK3 &= ~channel_mask;
}

int16_t read(){
    WheelGauge::disable();
    return motor_position;
    WheelGauge::enable();
}

int16_t milliturnsDistance(int16_t prev, int16_t current){
    return (int32_t)(current-prev)*1000/gearing/motor_counts_per_rev;
}

int16_t rawFromMilliturns(int16_t milliturns){
    return (int32_t)milliturns*gearing*motor_counts_per_rev/1000;
}

uint8_t encoderState(){
    return encoder_state;
    //return PIND & 0xF0;
}

}