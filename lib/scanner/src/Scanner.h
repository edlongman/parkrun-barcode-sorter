#include <avr/io.h>
#include <util/delay.h>

#ifndef _SCANNER_H
#define _SCANNER_H

namespace Scanner{

const uint8_t triggerPin = 6;

const uint16_t baud = 57600;

void init();
void enable();
void disable();
inline void startScan(){
    PORTC &= ~_BV(triggerPin);
}
inline void endScan(){
    PORTC |= _BV(triggerPin);
}
void scanLine(char* buffer, uint8_t length);
bool isScanComplete();

}

#endif /* _SCANNER_H */
