#include "PIController.h"

PIController::PIController(uint16_t _deci_freq){
    deci_freq = _deci_freq;
    if(deci_freq < (int16_t)deci_K_i) deci_K_i = deci_freq;
    if(deci_K_p>10) deci_K_p = 10;
}

void PIController::reset(){
    error_integral = 0;
}

void PIController::setSpeed(int16_t speed){
    reset();
    setpoint = speed;
    error_integral = 255*deci_freq/5/deci_K_i;
}

uint8_t PIController::stepControlEstimation(int16_t current_speed){
    int16_t error;
    __builtin_sub_overflow(setpoint, current_speed, &error);
    __builtin_add_overflow(error_integral, error, &error_integral);

    int16_t unity_integral_effort;
    __builtin_mul_overflow(error_integral,deci_K_i,&unity_integral_effort);
    uint16_t control_effort = 0;
    __builtin_add_overflow(unity_integral_effort/deci_freq, error/10*deci_K_p, &control_effort);
    if(control_effort>UINT8_MAX)return UINT8_MAX;
    else if(control_effort<0)return 0;
    return control_effort;
}