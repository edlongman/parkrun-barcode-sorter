#include <libopencm3/stm32/gpio.h>

#ifndef _SCANNER_H
#define _SCANNER_H

namespace Scanner{

const unsigned int triggerPin = GPIO14;

const uint16_t baud = 9600;

void init();
void enable();
void disable();
inline void startScan(){
    gpio_set(GPIOB, triggerPin);
}
inline void endScan(){
    gpio_clear(GPIOB, triggerPin);
}
void scanLine(char* buffer, uint8_t length);
bool isScanComplete();

}

#endif /* _SCANNER_H */
