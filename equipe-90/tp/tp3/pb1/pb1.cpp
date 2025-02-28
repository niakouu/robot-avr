#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

void allumerDelRouge()
{
    PORTA &= ~(1 << PA0);
    PORTA |= (1 << PA1);
}

int main()
{
    DDRA |= (1 << PA0) | (1 << PA1);
    PORTA &= ~(1 << PA0) & ~(1 << PA1);
    uint16_t b = 1000;
    uint16_t a = 0;
    while (true)
    {

        PORTA |= (1 << PORTA0);       
        for (uint16_t i = 0; i < b; i++)
        {
            _delay_us(1);
        }
        b--;

        PORTA &= ~(1 << PORTA0);
        for (uint16_t i = 0; i < a; i++)
        {
            _delay_us(1);
        }        
        a++;

        if (a == 1000){
            break;
        }
    }

    return 0;
}


