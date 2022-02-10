#include "PIController.h"
#include <NoOverflow.h>

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
    NoOverflow::sub(setpoint, current_speed, &error);
    int16_t new_integral;
    NoOverflow::add(error_integral, error, &new_integral);

    const uint8_t effort_ceiling = 2;
    if( alarm <= effort_ceiling ){
        error_integral = new_integral;
    }
    integral_observer = error_integral;
    error_observer = error;
    int16_t unity_integral_effort;
    // Divide by 10 to avoid limiting on common scenarios
    NoOverflow::mul(error_integral/10,deci_K_i,&unity_integral_effort);
    //integral_observer = error_integral;
    int16_t control_effort = 0;
    // Multiply by 10 to counteract previous divide
    NoOverflow::add(unity_integral_effort/(int16_t)deci_freq*10, error/10*(int16_t)deci_K_p, &control_effort);
    integral_observer = unity_integral_effort;
    
    // If max or min control effort, then put into alarm mode
    if(control_effort>UINT8_MAX){
        if(alarm<UINT8_MAX)
            alarm++;
        return UINT8_MAX;
    }
    else if(control_effort<0){
        if(setpoint>0)alarm++;
        return 0;
    }
    // Cooldown after alarm for short time
    const uint8_t cooldown_cycles = 1;
    if(alarm>cooldown_cycles)alarm=cooldown_cycles;
    else if(alarm>0)alarm--;
    return control_effort;
}