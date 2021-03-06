#include <stdint.h>

#ifndef _SCAN_INTERPRETER_H
#define _SCAN_INTERPRETER_H

namespace ScanInterpreter{

bool readPWMInteger(uint8_t &val, const char* buffer, const uint8_t length);

// Scan for "RSDIST <val>" from start of buffer
bool readResetDistance(int16_t &val, const char* buffer, const uint8_t length);

}

#endif /* _SCAN_INTERPRETER_H */
