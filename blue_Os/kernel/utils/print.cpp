#include "print.h"
#include <stdint.h>

int print(const char* str) {
    static uint16_t* video = (uint16_t*)0xb8000;
    static int row = 0, col = 0;
    while (*str) {
        if (*str == '\n') {
            row++;
            col = 0;
        } else {
            video[row * 80 + col] = (0x07 << 8) | *str;
            col++;
        }
        str++;
    }
    return 0;
}

int print_dec(uint32_t num) {
    char buf[16];
    int i = 0;
    if (num == 0) { print("0"); return 0; }
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    for (int j = i - 1; j >= 0; j--) {
        char s[2] = { buf[j], '\0' };
        print(s);
    }
    return 0;
}

int print_hex(uint32_t num) {
    const char* hex = "0123456789ABCDEF";
    print("0x");
    for (int i = 28; i >= 0; i -= 4) {
        char c[2] = { hex[(num >> i) & 0xF], '\0' };
        print(c);
    }
    return 0;
}