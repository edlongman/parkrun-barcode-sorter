#include <stdint.h>

#ifndef _PI_CONTROLLER
#define _PI_CONTROLLER

class PIController {
private: 
    uint16_t deci_freq = 10;
    int16_t setpoint;
    void reset();

    uint8_t deci_K_p = 7;
    uint8_t deci_K_i = 27;
public: 
    int16_t error_integral = 0;
    PIController(uint16_t deci_freq);
    void setSpeed(int16_t speed);
    uint8_t stepControlEstimation(int16_t current_speed);
};

#endif /* _PI_CONTROLLER */
