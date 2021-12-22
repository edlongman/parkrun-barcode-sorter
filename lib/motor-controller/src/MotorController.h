#include <stdint.h>

#ifndef _MOTOR_CONTROLLER_H
#define _MOTOR_CONTROLLER_H

namespace MotorController{

extern int16_t volatile observation;

void init();
void enable();
void disable();

// Set motor speed between min (0) and max(100)
void setPWM(uint8_t val);

int16_t setSpeed(int16_t val);
int16_t getSetpoint();
int16_t getSpeed();

}

#endif /* _MOTOR_CONTROLLER_H */
