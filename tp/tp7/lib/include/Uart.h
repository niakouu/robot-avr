#ifndef _UART_H
#define _UART_H

#include <stdio.h>

class Uart
{
public:
    Uart(/* args */);
    ~Uart();

    void transmit(char data);
    char receive();
    FILE* getStdout();

private:
    FILE stdout_;
    static int putChar(char data, FILE* stream);
};

#endif /* _UART_H */