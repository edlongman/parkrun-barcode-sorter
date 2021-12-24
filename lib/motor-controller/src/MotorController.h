#include <stdint.h>

#ifndef _MOTOR_CONTROLLER_H
#define _MOTOR_CONTROLLER_H

namespace MotorController{

extern int16_t volatile integral_observation;

void init();
void enable();
void disable();

// Set motor speed between min (0) and max(100)
void setPWM(uint8_t val);

int16_t setSpeed(int16_t val);
int16_t getSetpoint();
int16_t getSpeed();
void waitForPIStep();
void getPIStateVariables(int16_t& motorPosition, int16_t& speed, int16_t& error, int16_t& integral);

}

#endif /* _MOTOR_CONTROLLER_H */
