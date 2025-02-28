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
    DDRA |= (1 << PA0) | (1 << PA1)| (1 << PA2)| (1 << PA3);
    uint16_t b = 1000;
    uint16_t a = 0;
    while (true)
    {

        PORTA |= (1 << PORTA0);   //Enable Droit
        PORTA |= (1 << PORTA2);   //Enable Gauche
        PORTA |= (1 << PORTA1);   
        PORTA &= ~(1 << PORTA1); 
        PORTA |= (1 << PORTA3); //Enable Gauche        
        for (uint16_t i = 0; i < b; i++)
        {
            _delay_us(1);
        }
        b--;

        PORTA &= ~(1 << PORTA0);
        // PORTA &= ~(1 << PORTA2); 
        PORTA &= ~(1 << PORTA1); 
        // PORTA &= ~(1 << PORTA3);
        for (uint16_t i = 0; i < a; i++)
        {
            _delay_us(1);
        }        
        a++;

        if (a == 1000){
            break;
        }

        _delay_ms(1);
    }

    return 0;
}


