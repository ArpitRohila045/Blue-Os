#ifndef PORT_H
#define PORT_H
#include<stdint.h>

class Port {
    protected:
        uint16_t portnumber;
        Port(uint16_t portnumber);
        ~Port();
};

class Port8bit : public Port {
    public:
        Port8bit(uint16_t portnumber);
        ~Port8bit();
        virtual void write(uint8_t data);
        virtual uint8_t read();
};

class Port8bitSlow : public Port8bit {
    public:
        Port8bitSlow(uint16_t portnumber);
        ~Port8bitSlow();
        virtual void write(uint8_t data);
};

class Port16bit : public Port {
    public:
        Port16bit(uint16_t portnumber);
        ~Port16bit();
        virtual void write(uint16_t data);
        virtual uint16_t read();
};

class Port32bit : public Port {
    public:
        Port32bit(uint16_t portnumber);
        ~Port32bit();
        virtual void write(uint32_t data);
        virtual uint32_t read();
};

#endif