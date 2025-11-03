#ifndef __BLUE_OS__COMMON__PRINT_H
#define __BLUE_OS__COMMON__PRINT_H
#include "types.h"

namespace blueOs{
    namespace common{
        int print(char*);
        int print_dec(uint32_t);
        int print_hex(uint32_t);
        int putCharAt(int8_t,int8_t,char,int8_t);
    }
}

#endif