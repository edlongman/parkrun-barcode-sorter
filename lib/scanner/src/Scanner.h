#include <libopencm3/stm32/gpio.h>

#ifndef _SCANNER_H
#define _SCANNER_H

namespace Scanner{

const unsigned int triggerPin = GPIO14;

void init(unsigned int baud);
void enable();
void disable();
void startScan();
void endScan();
void scanLine(char* buffer, uint8_t length);
bool isScanComplete();

}

#endif /* _SCANNER_H */
