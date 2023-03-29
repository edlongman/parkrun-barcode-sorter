#include <stdint.h>

#ifndef _TOKEN_H
#define _TOKEN_H

class Token{
    int16_t insert_front;
    int16_t insert_back;
public:
    uint16_t value;
    uint8_t output;
    Token();
    Token(const int16_t initialPosition, const char* raw_input, const uint8_t base, const uint8_t sort_index);
    uint8_t getOutput();
    void setInsertBack(const int16_t frontPosition);
    int16_t getOpenDistance(const int16_t currentPositon);
    int16_t getCloseDistance(const int16_t currentPositon);

};

#endif /* _TOKEN_H */
