#include <stdint.h>

#ifndef _SERVO_CONTROLLER_H
#define _SERVO_CONTROLLER_H

namespace ServoController{

void init();
void enable();
void disable();
void setPWM(uint16_t val);

}

#endif /* _SERVO_CONTROLLER_H */
