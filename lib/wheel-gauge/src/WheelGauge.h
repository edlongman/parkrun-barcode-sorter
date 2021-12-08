#include <stdint.h>

#ifndef _WHEEL_GAUGE_H
#define _WHEEL_GAUGE_H

namespace WheelGauge{

const int16_t motor_counts_per_rev = 210*4;
const uint8_t gearing = 4;

void init();
void enable();
void disable();
int16_t read(); // return rotation of outside wheel in milliturns
float milliturnsScaling();
uint8_t encoderState();

}

#endif /* _WHEEL_GAUGE_H */
