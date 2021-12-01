#include <stdint.h>

#ifndef _MOTOR_CONTROLLER_H
#define _MOTOR_CONTROLLER_H

namespace MotorController{

void init();
void enable();
void disable();

// Set motor speed between min (0) and max(100)
void set(uint8_t val);

}

#endif /* _MOTOR_CONTROLLER_H */
