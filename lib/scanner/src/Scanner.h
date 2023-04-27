#include <libopencm3/stm32/gpio.h>

#ifndef _SCANNER_H
#define _SCANNER_H

namespace Scanner{

namespace CommandType{
    typedef enum {
        SUCCESS = 0x00,
        READ = 0x07,   
        WRITE = 0x08,
        SAVE = 0x09
    } command_type_num;
}

typedef uint8_t command_type;

const unsigned int triggerPin = GPIO14;

void init(unsigned int baud);
void changeSpeed(unsigned int baud, uint8_t* data_response);
void enable();
void disable();
void startScan();
void endScan();
void sendCommandForResponse(const command_type type, const uint16_t address, const uint8_t length, const uint8_t* send, uint8_t* response);
void scanLine(char* buffer, uint8_t length);
bool isScanComplete();
bool isCommandTxComplete();
bool isCommandRxComplete();

}

#endif /* _SCANNER_H */
