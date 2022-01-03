#include <stdint.h>

#ifndef _FLAP_GOVENOR_H
#define _FLAP_GOVENOR_H

namespace FlapGovenor{

void init();
void enable();
void disable();
void setOn(const uint8_t index);
void setOff(const uint8_t index);

}
#endif /* _FLAP_GOVENOR_H */
