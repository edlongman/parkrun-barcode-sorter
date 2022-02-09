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

// Servo controller used as coarse system timer
class Timer{
    private:
    uint16_t expiry_counts;
    uint8_t start;
    uint8_t last_check;
    public:
    // Create timer with min time of 1/servo_freq and max of timer period = 6.4s
    Timer(uint16_t duration_ms);
    // Latching check if timer has expired. Must be called twice in max timer period to be valid
    bool isExpired();
    void restart();
};

}

#endif /* _SERVO_CONTROLLER_H */
