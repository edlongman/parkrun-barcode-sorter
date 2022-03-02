#include <stdint.h>

#ifndef _WHEEL_GAUGE_H
#define _WHEEL_GAUGE_H

namespace WheelGauge{
#ifdef FIT0441_DRIVE
const int16_t motor_counts_per_rev = 45*6*2;
const uint8_t gearing_num = 80;
const uint8_t gearing_denom = 15;
#else
const int16_t motor_counts_per_rev = 210*4;
const uint8_t gearing_num = 80;
const uint8_t gearing_denom = 20;
#endif

void init();
void enable();
void disable();
int16_t read(); // return rotation of outside wheel in milliturns
int16_t milliturnsDistance(int16_t prev, int16_t current);
int16_t rawFromMilliturns(int16_t milliturns);
uint8_t encoderState();

}

#endif /* _WHEEL_GAUGE_H */
