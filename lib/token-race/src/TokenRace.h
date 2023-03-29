#include <stdint.h>
#include "Token.h"

#ifndef _TOKEN_RACE_H
#define _TOKEN_RACE_H

namespace TokenRace{

uint8_t getTokenCount();
bool checkFlapOutput(uint8_t flap, int16_t position);
Token* insertToken(char* token_read);
void popExpiredTokens(int16_t wheel_position);

}

#endif /* _TOKEN_RACE_H */
