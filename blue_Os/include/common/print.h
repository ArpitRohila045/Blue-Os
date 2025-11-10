#ifndef __BLUE_OS__COMMON__PRINT_H
#define __BLUE_OS__COMMON__PRINT_H
#include "types.h"

namespace blueOs{
    namespace common{
        void print(char*);
        int print_dec(uint32_t);
        void print_hex(uint8_t);
        int putCharAt(int8_t,int8_t,char,int8_t);
    }
}

#endif