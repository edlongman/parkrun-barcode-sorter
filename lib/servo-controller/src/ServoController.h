#include <stdint.h>

#ifndef _SERVO_CONTROLLER_H
#define _SERVO_CONTROLLER_H

namespace ServoController{

// Control servo movement state machine
enum struct State : uint8_t{
    collect,
    toInsert,
    insert,
    toCollect
};

void init();
void enable();
void disable();
void setPWM(uint16_t val);
void setTarget(State target);
State getState();
void enableStates();
void disableStates();

}

#endif /* _SERVO_CONTROLLER_H */
