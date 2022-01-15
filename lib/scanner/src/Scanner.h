#include <avr/io.h>
#include <util/delay.h>

#ifndef _SCANNER_H
#define _SCANNER_H

namespace Scanner{

const uint8_t nResetPin = 0;
const uint8_t triggerPin = 1;

void init(){
    PORTC |= _BV(nResetPin) | _BV(triggerPin); // Enable Reset Pull-up and set trigger to default high
    DDRC |= _BV(triggerPin); 
}

void startScan(){
    PORTC &= ~_BV(triggerPin);
}

void endScan(){
    PORTC |= _BV(triggerPin);
}

}

#endif /* _SCANNER_H */
