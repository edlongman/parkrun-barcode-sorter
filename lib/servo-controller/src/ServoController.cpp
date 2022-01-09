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

#define PULSE_CALC(microseconds) F_CPU/1000*microseconds/1000/prescaler;

struct ServoPos{
    static const uint16_t increment = 32;
    static const uint16_t min = PULSE_CALC(1706);
    static const uint16_t max = PULSE_CALC(2346);
    static const uint8_t insertSetTime = 12; // 250ms
    static const uint8_t insertDwellTime = 40;
    static const uint8_t collectSetTime = 4;
    static const uint8_t collectDwellTime = 10;
};

volatile State stateTarget=State::collect;
volatile State state = State::collect;
ISR(TIMER1_COMPB_vect){
    static uint8_t dwell_timer=0;
    static uint16_t servoPosition = 0;
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
    uint16_t newPos = 0;
    uint8_t new_dwell_timer = 0;
    switch(state){
        case State::collect:
            servoPosition = ServoPos::max;
            //NoOverflow::add(dwell_timer, 1, &new_dwell_timer);
            new_dwell_timer += dwell_timer + 1;
            if(new_dwell_timer<dwell_timer)new_dwell_timer = UINT8_MAX;
            dwell_timer = new_dwell_timer;
            break;
        case State::toInsert:
            NoOverflow::sub(servoPosition, ServoPos::increment, &newPos);
            servoPosition = newPos;
            break;
        case State::insert:
            servoPosition = ServoPos::min;
            //NoOverflow::add(dwell_timer, 1, &new_dwell_timer);
            new_dwell_timer += dwell_timer + 1;
            if(new_dwell_timer<dwell_timer)new_dwell_timer = UINT8_MAX;
            dwell_timer = new_dwell_timer;
            break;
        case State::toCollect:
            NoOverflow::add(servoPosition, ServoPos::increment, &newPos);
            servoPosition = newPos;
            break;
    }

    
    // Anti overheat code
    if(state == State::collect && dwell_timer > ServoPos::collectSetTime
        || state == State::insert && dwell_timer > ServoPos::insertSetTime){
        DDRD &= ~_BV(4);
        //setPWM(0);
    }
    else{
        DDRD |= _BV(4);
        setPWM(servoPosition);
    }

    

}

void init(){
    // ATMega644P/V Datasheet p167 17.12.3
    // Fast PWM 
    TCCR1A = _BV(WGM11) | _BV(WGM10);
    // Prescaler of 8
    TCCR1B =  _BV(WGM12) | _BV(WGM13) | _BV(CS11);

    OCR1A = TIMER_TOP(40, prescaler);
    OCR1B = 0;

}

void enable(){
    TCCR1A |= _BV(COM1B1);
    DDRD |= _BV(PD4);
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
    DDRD &= ~_BV(PD4);
}

void setPWM(uint16_t val){
    if(val<2240){
        val = 0;
    }
    else if(val<2560){
        val = 2560;
    }
    else if(val>3520){
        val = 3520;
    }
    uint8_t sreg;
    sreg = SREG;
    cli();
    OCR1B = val;
    SREG = sreg;
}

}