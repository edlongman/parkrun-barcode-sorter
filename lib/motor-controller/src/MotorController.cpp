#include "MotorController.h"
#include <WheelGauge.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <PIController.h>

namespace MotorController{

#define PHASE_CORRECT_TOP(FREQ, PRESCALE) (float)(F_CPU)/PRESCALE/FREQ/2

#define PWM_CALC(percentage) (uint16_t)(timer_top)*percentage/255

const uint16_t timer_frequency = 24000;
const uint8_t control_frequency = 50;
const uint8_t default_pwm_pc = 10;
const uint8_t prescaler = 1;
const uint8_t timer_top = PHASE_CORRECT_TOP(timer_frequency, prescaler);

static int16_t volatile raw_speed = 0;

// Used when switching mode and in interrupt
// to control if PI Control is active or just pwm
static volatile bool PI_control_active = 0;
static volatile PIController controller{control_frequency*10};

// Internal PWM value access
void _setPWM(uint8_t val){
    disable();
    OCR2B = PWM_CALC(val);
    enable();
}

int16_t volatile observation = 0;

static int16_t motor_position = 0;
static uint16_t interrupt_prescaler = 0;
// Motor Controller interrupt with downscaled control check
ISR(TIMER2_OVF_vect){
    const uint16_t interrupt_count_top = timer_frequency/control_frequency;
    if(++interrupt_prescaler >= interrupt_count_top){
        interrupt_prescaler = 0;
        int16_t new_motor_position = WheelGauge::read();
        if(PI_control_active){
            int16_t motor_difference = new_motor_position - motor_position;
            int16_t speed;
            __builtin_mul_overflow(motor_difference, (int16_t)control_frequency, &speed);
            raw_speed = speed;
            _setPWM(controller.stepControlEstimation(speed, observation));
        }
        motor_position = new_motor_position;
        PINB |= _BV(PIN7);
    }
}


void init(){
    // Intialize Timer at 10% Duty cycle
    // Fast PWM mode, count to OCR2A, clear OC2B at BOTTOM, set at Compare Match
    TCCR2A = _BV(WGM20) | _BV(WGM22) | _BV(COM2B1);
    // prescaler /8
    TCCR2B = _BV(WGM22) | _BV(CS20);
    // Count to 79 for 2.5kHz @ 12MHz F_CPU (80-1)
    OCR2A = timer_top;
    TIMSK2 = 0;
    OCR2B = PWM_CALC(default_pwm_pc);
    DDRD |=  _BV(PIN6);
    DDRB |= _BV(PIN7);
}

void enable(){
    TIMSK2 |= _BV(TOIE2);
}

void disable(){
    TIMSK2 &= ~_BV(TOIE2);
}

void setPWM(uint8_t val){
    PI_control_active = 0;
    _setPWM(val);
}

int16_t setSpeed(int16_t milliturns_per_sec){
    int16_t encoder_per_sec = WheelGauge::rawFromMilliturns(milliturns_per_sec);
    disable();
    int16_t ret = controller.setSpeed(encoder_per_sec);
    enable();
    PI_control_active = 1;
    return ret;
}

int16_t getSetpoint(){
    return controller.getSetpoint();
}

int16_t getSpeed(){
    MotorController::disable();
    int16_t speed_cache =  raw_speed;
    MotorController::enable();
    return speed_cache;
}

}