#include "MotorController.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Util.h"

namespace MotorController{

#define PWM_CALC(percentage) (uint16_t)(timer_top)*percentage/100

const uint16_t timer_frequency = 2500;
const uint8_t control_frequency = 25;
const uint8_t default_pwm_pc = 10;
const uint8_t prescaler = 32;
const uint8_t timer_top = TIMER_TOP(timer_frequency, prescaler);

static uint8_t interrupt_prescaler = 0;
ISR(TIMER2_OVF_vect){
    const uint8_t interrupt_count_top = timer_frequency/control_frequency;
    if(++interrupt_prescaler >= interrupt_count_top){
        interrupt_prescaler = 0;
        PIND |= _BV(PIN5);
    }
}
void init(){
    // Intialize Timer at 10% Duty cycle
    // Fast PWM mode, count to OCR2A, clear OC2B at BOTTOM, set at Compare Match
    TCCR2A = _BV(WGM20) | _BV(WGM21) | _BV(WGM22) | _BV(COM2B1);
    // prescaler /32 
    TCCR2B = _BV(WGM22) | _BV(CS21) | _BV(CS20);
    // Count to 149 for 2.5kHz @ 12MHz F_CPU (150-1)
    OCR2A = timer_top;
    TIMSK2 = 0;
    OCR2B = PWM_CALC(default_pwm_pc);
    DDRD |= _BV(PIN5) | _BV(PIN6);
}

void enable(){
    TIMSK2 |= _BV(TOIE2);
}
void disable(){
    TIMSK2 &= ~_BV(TOIE2);
}

void set(uint8_t val){
    disable();
    OCR2B = PWM_CALC(val);
    enable();
}

}