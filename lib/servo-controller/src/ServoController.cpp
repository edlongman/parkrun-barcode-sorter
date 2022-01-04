#include <stdint.h>
#include <ServoController.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <NoOverflow.h>

namespace ServoController{

// Calculate the timer top value using floating point arithmetic and rounding
#define TIMER_TOP(FREQ, PRESCALE) (float)(F_CPU)/PRESCALE/FREQ-0.5

const uint16_t timer_frequency = 40;
const uint8_t prescaler = 8;
const uint16_t timer_top = TIMER_TOP(timer_frequency, prescaler);

#define PWM_CALC(proportion) (uint32_t)timer_top*proportion/UINT16_MAX;

struct ServoPos{
    static const uint8_t increment = 1;
    static const uint8_t min = 80;
    static const uint8_t max = 110;
    static const uint8_t insertDwellTime = 30; // half second at 44Hz
    static const uint8_t collectDwellTime = 10;
};

volatile State stateTarget=State::collect;
volatile State state = State::collect;
ISR(TIMER1_COMPB_vect){
    static uint8_t dwell_timer=0;
    static uint8_t servoPosition = 0;
    // Next State logic
    switch(state){
        case State::collect:
            if(dwell_timer >= ServoPos::collectDwellTime
              && stateTarget == State::insert){
                state = State::toInsert;
            }
            break;
        case State::toInsert:
            if(servoPosition <= ServoPos::min){
                state = State::insert;
                dwell_timer = 0;
            }
            break;
        case State::insert:
            if(dwell_timer >= ServoPos::insertDwellTime
              && stateTarget == State::collect){
                state = State::toCollect;
            }
            break;
        case State::toCollect:
            if(servoPosition >= ServoPos::max){
                state = State::collect;
                dwell_timer = 0;
            }
            break;
    }

    // State Consequences
    uint8_t newPos = 0;
    uint8_t new_dwell_timer = 0;
    switch(state){
        case State::collect:
            servoPosition = ServoPos::max;
            NoOverflow::add(dwell_timer, 1, &new_dwell_timer);
            dwell_timer = new_dwell_timer;
            break;
        case State::toInsert:
            NoOverflow::sub(servoPosition, ServoPos::increment, &newPos);
            servoPosition = newPos;
            break;
        case State::insert:
            servoPosition = ServoPos::min;
            NoOverflow::add(dwell_timer, 1, &new_dwell_timer);
            dwell_timer = new_dwell_timer;
            break;
        case State::toCollect:
            NoOverflow::add(servoPosition, ServoPos::increment, &newPos);
            servoPosition = newPos;
            break;
    }

    // Anti overheat code
    if(state == State::collect && dwell_timer > ServoPos::collectDwellTime
        || state == State::insert && dwell_timer > ServoPos::insertDwellTime/2){
        disable();
    }
    else{
        enable();
    }

    setPWM(servoPosition);

}

void init(){
    // Initialize servo timer
    // 9 bit Fast PWM mode
    TCCR1A = _BV(WGM11) | _BV(WGM10);
    // Prescaler of 1024
    TCCR1B =  _BV(WGM12) | _BV(CS12);

    OCR1B = ServoPos::max;

    DDRD |= _BV(PD4);
}

void enable(){
    TCCR1A |= _BV(COM1B1);
}

void setTarget(State target){
    stateTarget = target;
}
State getState(){
    return state;
}

void enableStates(){
    TIMSK1 |= _BV(OCIE1B);
}

void disableStates(){
    TIMSK1 &= ~_BV(OCIE1B);
}

void disable(){
    TCCR1A &= ~_BV(COM1B1);
}

void setPWM(uint16_t val){
    if(val<70){
        val = 0;
    }
    else if(val<80){
        val = 80;
    }
    else if(val>110){
        val = 110;
    }
    uint8_t sreg;
    sreg = SREG;
    cli();
    OCR1B = val;
    SREG = sreg;
}

}