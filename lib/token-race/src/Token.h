#include <stdint.h>

#ifndef _TOKEN_H
#define _TOKEN_H

class Token{
    int16_t intial_position;
public:
    uint16_t value;
    uint8_t output;
    Token();
    Token(const int16_t initialPosition, const char* raw_input, const uint8_t base, const uint8_t sort_index);
    uint8_t getOutput();
    int16_t getDistance(const int16_t currentPositon);

};

#endif /* _TOKEN_H */
