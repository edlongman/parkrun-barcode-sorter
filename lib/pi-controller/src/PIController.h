#include <stdint.h>

#ifndef _PI_CONTROLLER_H
#define _PI_CONTROLLER_H

bool mul_no_overflow(int16_t a, int16_t b, int16_t* res);

class PIController {
private: 
    uint16_t deci_freq = 50;
    int16_t setpoint;
    void reset() volatile;

    uint8_t deci_K_p = 2; //7
    uint8_t deci_K_i = 27; //27
public: 
    int16_t error_integral = 0;
    PIController(uint16_t deci_freq);
    int16_t setSpeed(int16_t speed) volatile;
    int16_t getSetpoint() volatile;
    uint8_t stepControlEstimation(int16_t current_speed, int16_t volatile &error_observer, int16_t volatile &integral_observer) volatile;
};

#endif /* _PI_CONTROLLER */
