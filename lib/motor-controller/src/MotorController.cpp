#include "MotorController.h"
#include <WheelGauge.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <PIController.h>
#include <NoOverflow.h>

#define Board_LED_Pin                                GPIO13
#define Board_LED_GPIO_Port                          GPIOC

namespace MotorController{

#define PHASE_CORRECT_TOP(FREQ, PRESCALE) (F_CPU)/PRESCALE/FREQ/4

#define PWM_CALC(percentage) (uint32_t)(timer_top)*percentage/UINT8_MAX

const uint16_t timer_frequency = 24000;
const uint8_t control_frequency = 50;
const uint8_t default_pwm_level = 0;
const uint8_t prescaler = 1;
const uint16_t timer_top = PHASE_CORRECT_TOP(timer_frequency, prescaler);

static int16_t volatile raw_speed = 0;

// Used when switching mode and in interrupt
// to control if PI Control is active or just pwm
static volatile bool PI_control_active = 0;
static volatile PIController controller{control_frequency*10};

// Internal PWM value access in per 256
void _setPWM(uint8_t val){
    if(val<40&&val>0){
        // Motor stall prevention 
        // kicking in at very low torque
        //val = 0;
    }
    disable();
    timer_set_oc_value(TIM1, TIM_OC1, PWM_CALC(val));
    enable();
}

int16_t volatile integral_observation = 0;
int16_t volatile error_observation = 0;

static int16_t motor_position = 0;
static uint16_t volatile interrupt_prescaler = 0;

}

// Motor Controller interrupt with downscaled control check
void tim1_cc_isr(){
    timer_clear_flag(TIM1, TIM_SR_CC1IF);
    gpio_toggle(Board_LED_GPIO_Port, Board_LED_Pin);
    /*const uint16_t interrupt_count_top = timer_frequency/control_frequency;
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
    }*/
}

namespace MotorController{

void init(){
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOA);

	gpio_set(Board_LED_GPIO_Port, Board_LED_Pin);
	gpio_set_mode(Board_LED_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, Board_LED_Pin);

	nvic_enable_irq(NVIC_TIM1_CC_IRQ);
	//nvic_set_priority(NVIC_TIM1_UP_IRQ, 16);

    // Intialize Timer at 10% Duty cycle
    // Phase correct PWM mode, count to OCR2A
	rcc_periph_clock_enable(RCC_TIM1);
    rcc_periph_clock_enable(RCC_AFIO);

	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
				TIM_CR1_DIR_UP);
    timer_enable_oc_output(TIM1, TIM_OC1);
    timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_TOGGLE);

    gpio_set_mode(GPIO_BANK_TIM1_CH1, 
                GPIO_MODE_OUTPUT_2_MHZ, 
                GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_TIM1_CH1);

    timer_set_oc_polarity_high(TIM1, TIM_OC1);
    #ifdef FIT0441_DRIVE
        timer_set_oc_polarity_high(TIM1, TIM_OC1);
    #endif
    // prescaler /1
	timer_set_prescaler(TIM1, 1000); // Timer peripheral clock 24MHz
    // Count to 500 for 24kHz
	timer_set_period(TIM1, timer_top);
    timer_clear_flag(TIM1, TIM_SR_CC1IF);
    timer_set_oc_value(TIM1, TIM_OC1, PWM_CALC(default_pwm_level));
    timer_enable_counter(TIM1);
}

void enable(){
    timer_enable_irq(TIM1, TIM_DIER_CC1IE);
}

void disable(){
    timer_disable_irq(TIM1, TIM_DIER_CC1IE);
}

void setPWM(uint8_t val){
    PI_control_active = 0;
    _setPWM(val);
}

uint8_t getPWM(){
    return TIM_CCR1(TIM1)*100/timer_top;
}
uint8_t getTimer(){
    return timer_get_counter(TIM1)*100/timer_top;
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