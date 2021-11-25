#ifndef _UTIL_H
#define _UTIL_H

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// Calculate the timer top value using floating point arithmetic and rounding
#define TIMER_TOP(FREQ, PRESCALE) (float)(F_CPU)/PRESCALE/FREQ-0.5

#endif /* _UTIL_H */
