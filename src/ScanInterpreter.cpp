#include <string.h>
#include "Util.h"
#include "ScanInterpreter.h"

namespace ScanInterpreter{
    
bool readPWMInteger(uint8_t &val, const char* buffer, const uint8_t length_lim){
    const uint8_t command_length = 4;
    if (strcasestr(buffer, "pwm ") == buffer){
        uint8_t setpoint = 0;
        uint8_t rx_pos = command_length;
        while(buffer[rx_pos]>='0' && buffer[rx_pos]<='9' && rx_pos < length_lim){
            setpoint = setpoint * 10 + buffer[rx_pos] - '0';
            rx_pos ++;
        }
        if(rx_pos > command_length){
            val = MIN(setpoint, 100);
            return true;
        }
    }
    return false;
}

}