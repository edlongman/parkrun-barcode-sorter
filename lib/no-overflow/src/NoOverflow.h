#include <stdint.h>

#ifndef _NO_OVERFLOW_H
#define _NO_OVERFLOW_H

namespace NoOverflow{

inline bool mul(const int16_t a, const int16_t b, int16_t* const res){
    if(__builtin_mul_overflow(a, b, res)){
        if( (a > 0 && b > 0) || (a < 0 && b < 0) ){
            *res = INT16_MAX;
        }
        else *res = INT16_MIN;
        return true; 
    }
    return false;
};

inline bool add(const int16_t a, const int16_t b, int16_t* const res){
    if(__builtin_add_overflow(a, b, res)){
        if(*res<0)*res=INT16_MAX;
        else *res=INT16_MIN;
        return true;
    }
    return false;
}

inline bool add(const uint8_t a, const uint8_t b, uint8_t* const res){
    if(__builtin_add_overflow(a, b, res)){
        *res=UINT8_MAX;
        return true;
    }
    return false;
}

inline bool sub(const int16_t a, const int16_t b, int16_t* const res){
    if(__builtin_sub_overflow(a, b, res)){
        if(*res<0)*res=INT16_MAX;
        else *res=INT16_MIN;
        return true;
    }
    return false;
}

inline bool sub(const uint8_t a, const uint8_t b, uint8_t* const res){
    if(__builtin_sub_overflow(a, b, res)){
        *res=0;
        return true;
    }
    return false;
}

}
#endif /* _NO_OVERFLOW_H */
