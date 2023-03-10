#include "WheelGauge.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

namespace WheelGauge{

void init(){
    
	rcc_periph_clock_enable(RCC_TIM3);
    rcc_periph_clock_enable(RCC_AFIO);

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO6 | GPIO7);
    gpio_clear(GPIOA, GPIO6 | GPIO7);

    // Use timer 3 as encoder counter
    timer_ic_set_input(TIM3, TIM_IC1, TIM_IC_IN_TI1);
    #ifdef FIT0441_DRIVE
        timer_slave_set_mode(TIM3, TIM_SMCR_SMS_ECM1);
        timer_slave_set_trigger(TIM3, TIM_SMCR_TS_TI1FP1);
    #else
        timer_ic_set_input(TIM3, TIM_IC2, TIM_IC_IN_TI2);
        timer_slave_set_mode(TIM3, TIM_SMCR_SMS_EM3);
    #endif

}

void enable(){
    timer_enable_counter(TIM3);
}

void disable(){
    timer_disable_counter(TIM3);
}

void teardown(){
}

int16_t read(){
    return timer_get_counter(TIM3);
}

int16_t milliturnsDistance(int16_t prev, int16_t current){
    return (int32_t)(current-prev)*1000*gearing_denom/gearing_num/motor_counts_per_rev;
}

int16_t rawFromMilliturns(int16_t milliturns){
    return (int32_t)milliturns*gearing_num/gearing_denom*motor_counts_per_rev/1000;
}

}