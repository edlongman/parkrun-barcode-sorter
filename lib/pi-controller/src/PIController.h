#include <stdint.h>

#ifndef _PI_CONTROLLER
#define _PI_CONTROLLER

class PIController {
private: 
    uint16_t deci_freq = 50;
    int16_t setpoint;
    void reset() volatile;

    uint8_t deci_K_p = 2; //7
    uint8_t deci_K_i = 8; //27
public: 
    int16_t error_integral = 0;
    PIController(uint16_t deci_freq);
    int16_t setSpeed(int16_t speed) volatile;
    int16_t getSetpoint() volatile;
    uint8_t stepControlEstimation(int16_t current_speed, volatile int16_t &internal_observation) volatile;
};

#endif /* _PI_CONTROLLER */
