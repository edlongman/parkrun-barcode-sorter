#include "MotorController.h"
#include <WheelGauge.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <PIController.h>
#include <NoOverflow.h>

namespace MotorController{

#define PHASE_CORRECT_TOP(FREQ, PRESCALE) (F_CPU)/PRESCALE/FREQ/2

#define PWM_CALC(percentage) (uint16_t)(timer_top)*percentage/UINT8_MAX;

const uint16_t timer_frequency = 24000;
const uint8_t control_frequency = 50;
const uint8_t default_pwm_level = 0;
const uint8_t prescaler = 1;
const uint8_t timer_top = PHASE_CORRECT_TOP(timer_frequency, prescaler);

static int16_t volatile raw_speed = 0;

// Used when switching mode and in interrupt
// to control if PI Control is active or just pwm
static volatile bool PI_control_active = 0;
static volatile PIController controller{control_frequency*10};

// Internal PWM value access
void _setPWM(uint8_t val){
    if(val<40&&val>0){
        // Prevent motor stall prevention 
        // kicking in at very low torque
        val = 0;
    }
    disable();
    OCR2B = PWM_CALC(val);
    enable();
}

int16_t volatile integral_observation = 0;
int16_t volatile error_observation = 0;

static int16_t motor_position = 0;
static uint16_t volatile interrupt_prescaler = 0;
// Motor Controller interrupt with downscaled control check
ISR(TIMER2_OVF_vect){
    const uint16_t interrupt_count_top = timer_frequency/control_frequency;
    if(++interrupt_prescaler >= interrupt_count_top){
        interrupt_prescaler = 0;
        int16_t new_motor_position = WheelGauge::read();
        if(PI_control_active){
            int16_t motor_difference = new_motor_position - motor_position;
            int16_t speed;
            NoOverflow::mul(motor_difference, (int16_t)control_frequency, &speed);
            raw_speed = speed;
            _setPWM(controller.stepControlEstimation(speed, error_observation, integral_observation));
        }
        motor_position = new_motor_position;
    }
}


void init(){
    // Intialize Timer at 10% Duty cycle
    // Phase correct PWM mode, count to OCR2A
    TCCR2A = _BV(WGM20) | _BV(WGM22) | _BV(COM2B1);
    #ifdef FIT0441_DRIVE
        TCCR2A |= _BV(COM2B0);// Invert PWM Output
    #endif
    // prescaler /1
    TCCR2B = _BV(WGM22) | _BV(CS20);
    // Count to 250 for 24kHz
    OCR2A = timer_top;
    TIMSK2 = 0;
    OCR2B = PWM_CALC(default_pwm_level);
    DDRD |=  _BV(PIN6);
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

void waitForPIStep(){
    while(interrupt_prescaler == 0);// Catch rare case when called again very quickly
    while(interrupt_prescaler);
}

void getPIStateVariables(int16_t& motorPosition, int16_t& speed, int16_t& error, int16_t& integral){
    MotorController::disable();
    motorPosition = WheelGauge::read();
    speed = raw_speed;
    error = error_observation;
    integral = integral_observation;
    MotorController::enable();
}

}