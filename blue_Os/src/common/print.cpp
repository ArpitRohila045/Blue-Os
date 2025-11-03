#include <common/print.h>
#include <common/types.h>

namespace blueOs {
namespace common {

int print(char* str) {
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
    if (num == 0) { print((char*)"0"); return 0; }
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
    print((char*)"0x");
    for (int i = 28; i >= 0; i -= 4) {
        char c[2] = { hex[(num >> i) & 0xF], '\0' };
        print(c);
    }
    return 0;
}

int putCharAt(int8_t x, int8_t y, char c, int8_t color) {
    uint16_t* videoMemory = (uint16_t*)0xB8000;
    videoMemory[80 * y + x] = (color << 8) | c;
    return 0;
}

} // namespace common
} // namespace blueOs
