#ifndef _UART_H
#define _UART_H

#include <stdio.h>

class Uart {
public:
    Uart(Uart&) = delete;
    void operator=(const Uart&) = delete;

    void transmit(uint8_t data);
    uint8_t receive();
    FILE* getEmulatedFile();

private:
    Uart();
    ~Uart();
    FILE emulatedFile_;
    static int putChar(char data, FILE* stream);
    static int getChar(FILE* stream);
};

#endif /* _UART_H */