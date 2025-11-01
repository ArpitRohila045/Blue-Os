#ifndef PRINT_H
#define PRINT_H
#include<stdint.h>

int print(const char* str);
int print_dec(uint32_t num);
int print_hex(uint32_t num);
int putCharAt(const int8_t x, const int8_t y, const char ch, const int8_t color);
#endif