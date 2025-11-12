#include <common/print.h>
#include <common/types.h>

namespace blueOs {
    namespace common {

        void print(char* str) {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;

            static uint8_t x=0,y=0;

            for(int i = 0; str[i] != '\0'; ++i)
            {
                switch(str[i])
                {
                    case '\n':
                        x = 0;
                        y++;
                        break;
                    default:
                        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                        x++;
                        break;
                }

                if(x >= 80)
                {
                    x = 0;
                    y++;
                }

                if(y >= 25)
                {
                    for(y = 0; y < 25; y++)
                        for(x = 0; x < 80; x++)
                            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                    x = 0;
                    y = 0;
                }
            }
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

        void print_hex(uint8_t key) {
                char* foo = "00";
                char* hex = "0123456789ABCDEF";
                foo[0] = hex[(key >> 4) & 0xF];
                foo[1] = hex[key & 0xF];
                print(foo);
        }

        void print_hex32(uint32_t key){
            char* foo = "00000000";
            char* hex = "0123456789ABCDEF";
            foo[0] = hex[(key >> 28) & 0xF];
            foo[1] = hex[(key >> 24) & 0xF];
            foo[2] = hex[(key >> 20) & 0xF];
            foo[3] = hex[(key >> 16) & 0xF];
            foo[4] = hex[(key >> 12) & 0xF];
            foo[5] = hex[(key >> 8) & 0xF];
            foo[6] = hex[(key >> 4) & 0xF];
            foo[7] = hex[key & 0xF];
            print(foo);
        }
        
        int putCharAt(int8_t x, int8_t y, char c, int8_t color) {
            uint16_t* videoMemory = (uint16_t*)0xB8000;
            videoMemory[80 * y + x] = (color << 8) | c;
            return 0;
        }

    } // namespace common
} // namespace blueOs
