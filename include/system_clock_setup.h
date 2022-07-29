/** 
 * This file defines the system clock configuration 
 * and the system clock update function **/


#ifndef __SYSTEM_CLOCK_SETUP_H
#define __SYSTEM_CLOCK_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

void SystemClock_Config(void);
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif // __SYSTEM_CLOCK_SETUP_H