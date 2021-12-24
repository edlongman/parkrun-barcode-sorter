#include "PIController.h"

PIController::PIController(uint16_t _deci_freq){
    deci_freq = _deci_freq;
    if(deci_freq < (int16_t)deci_K_i) deci_K_i = deci_freq;
    if(deci_freq < 10) deci_K_i = 10;
    if(deci_K_p>10) deci_K_p = 10;
}

void PIController::reset() volatile{
    error_integral = 0;
}

int16_t PIController::setSpeed(int16_t speed) volatile{
    reset();
    setpoint = speed;
    error_integral = (int16_t)deci_freq/5*255/deci_K_i;
    return error_integral;
}

int16_t PIController::getSetpoint() volatile{
    return setpoint;
}

uint8_t PIController::stepControlEstimation(int16_t current_speed, int16_t volatile &error_observer, int16_t volatile &integral_observer) volatile{
    int16_t error;
    __builtin_sub_overflow(setpoint, current_speed, &error);
    int16_t new_integral;
    __builtin_add_overflow(error_integral, error, &new_integral);
    error_integral = new_integral;
    int16_t unity_integral_effort;
    // Divide by 10 to avoid limiting on common scenarios
    __builtin_mul_overflow(error_integral/10,deci_K_i,&unity_integral_effort);
    integral_observer = error_integral;
    int16_t control_effort = 0;
    // Multiply by 10 to counteract previous divide
    __builtin_add_overflow(unity_integral_effort*10/(int16_t)deci_freq, error/10*(int16_t)deci_K_p, &control_effort);
    integral_observer = unity_integral_effort*10/(int16_t)deci_freq;
    error_observer = error/10*(int16_t)deci_K_p;
    if(control_effort>UINT8_MAX)return UINT8_MAX;
    else if(control_effort<0)return 0;
    return control_effort;
}