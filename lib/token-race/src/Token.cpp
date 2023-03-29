#include "Token.h"
#include <WheelGauge.h>

Token::Token(){
    insert_front = 0;
    insert_back = 0;
    value = 0;
    output = 255;
}

// Stack safe power implementation for small integers
uint16_t spow(const uint8_t base, uint8_t exponent){
    uint16_t accum = 1;
    for(uint8_t i=0;i<exponent;i++){
        accum *= base;
    }
    return accum;
}

Token::Token(const int16_t initialPosition, const char* raw_input, const uint8_t base, const uint8_t sort_index){
    insert_front = initialPosition;
    setInsertBack(initialPosition);
    int16_t accum = 0;
    if(raw_input[0] == 'P'){
        for(int i=1;i<6;i++){
            if(raw_input[i]<='9' && raw_input[i]>='0'){
                accum = accum*10 + raw_input[i] - '0';
            }
            else{
                break;
            }
        }
    }
    value = accum;
    const uint16_t base_column = spow(base, sort_index);
    output = value/base_column%base;
}

void Token::setInsertBack(int16_t frontPosition){
    // Token is 85 milliTurns wide
    insert_back = frontPosition - WheelGauge::rawFromMilliturns(85);
}

int16_t Token::getOpenDistance(int16_t current_position){
    return WheelGauge::milliturnsDistance(insert_front, current_position);
}

int16_t Token::getCloseDistance(int16_t current_position){
    return WheelGauge::milliturnsDistance(insert_back, current_position);
}

uint8_t Token::getOutput(){
    return output;
}