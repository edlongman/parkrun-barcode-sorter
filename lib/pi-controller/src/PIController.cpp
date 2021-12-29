#include "PIController.h"

bool mul_no_overflow(int16_t a, int16_t b, int16_t* res){
    if(__builtin_mul_overflow(a, b, res)){
        if( (a > 0 && b > 0) || (a < 0 && b < 0) ){
            *res = INT16_MAX;
        }
        else *res = INT16_MIN;
        return true; 
    }
    return false;
}

bool add_no_overflow(int16_t a, int16_t b, int16_t* res){
    if(__builtin_add_overflow(a, b, res)){
        if(*res<0)*res=INT16_MAX;
        else *res=INT16_MIN;
        return true;
    }
    return false;
}

bool sub_no_overflow(int16_t a, int16_t b, int16_t* res){
    if(__builtin_sub_overflow(a, b, res)){
        if(*res<0)*res=INT16_MAX;
        else *res=INT16_MIN;
        return true;
    }
    return false;
}

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
    sub_no_overflow(setpoint, current_speed, &error);
    int16_t new_integral;
    add_no_overflow(error_integral, error, &new_integral);

    error_integral = new_integral;
    integral_observer = error_integral;
    error_observer = error;
    int16_t unity_integral_effort;
    // Divide by 10 to avoid limiting on common scenarios
    mul_no_overflow(error_integral/10,deci_K_i,&unity_integral_effort);
    //integral_observer = error_integral;
    int16_t control_effort = 0;
    // Multiply by 10 to counteract previous divide
    add_no_overflow(unity_integral_effort/(int16_t)deci_freq*10, error/10*(int16_t)deci_K_p, &control_effort);
    integral_observer = unity_integral_effort;//*10/(int16_t)deci_freq
    //error_observer = error/10*(int16_t)deci_K_p;
    //error_observer = control_effort;
    if(control_effort>UINT8_MAX)return UINT8_MAX;
    else if(control_effort<0)return 0;
    return control_effort;
}