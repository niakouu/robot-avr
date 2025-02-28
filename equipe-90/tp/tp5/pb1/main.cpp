/*
Description du programme:
    Notre programme écrit la chaine de charactère  POLYTECHNIQUE MONTREAL à l'addresse  0x0000 et la lit  pour vérfier. Lorsque la chaine est la même la DEL
    s'allume verte et si la chaine est différente la DEL s'allume rouge.

Identification matérielle:
    PINA0 est le - de la DEL.
    PINA1 est le + de la DEL.
    Placer le cavalier MemEn
*/

#define F_CPU 8000000UL
#define SIZE_STRING 23
#define FIRST_ADRESS_MEMORY 0x0000

#include "memoire_24.h"
#include <avr/io.h>
#include <util/delay.h>

void turnLedOnGreen() {
    PORTA |= (1 << PA0);
    PORTA &= ~(1 << PA1);
}

void turnLedOnRed() {
    PORTA |= (1 << PA1);
    PORTA &= ~(1 << PA0);
}

void turnOffLed() {
    PORTA &= ~(1 << PA0 | 1 << PA1);
}

int main() {
    DDRA |= (1 << PINA0) | (1 << PINA1);
    Memoire24CXXX memory;
    uint8_t string[SIZE_STRING] = {'P', 'O', 'L', 'Y', 'T', 'E', 'C', 'H', 'N', 'I', 'Q', 'U', 'E', ' ', 'M', 'O', 'N', 'T', 'R', 'E', 'A', 'L', '\0'};
    memory.ecriture(FIRST_ADRESS_MEMORY, string, SIZE_STRING);
    _delay_ms(5);
    uint8_t newString[SIZE_STRING] = {};
    memory.lecture(FIRST_ADRESS_MEMORY, newString, SIZE_STRING);

    for (uint8_t i = 0; i < SIZE_STRING; i++) {
        if (string[i] != newString[i]) {
            turnLedOnRed();
            return 0;
        }
    }
    
    turnLedOnGreen();
    return 0;
}
