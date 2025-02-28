/*
Description du programme:
    Notre programme lit le contenu de la mémoire jusqu'à un charactère '\0'. Il l'envoie ensuite via UART au ATMega8, qui lui-même le transmettra via le cable usb à l'ordinateur.

Identification matérielle:
    Brancher la carte mère avec le port usb à l'ordinateur
    Placer le cavalier DbgEN
    Placer le cavalier MemEn

*/

#define F_CPU 8000000UL
#define FIRST_ADRESS_MEMORY 0x0000

#include "memoire_24.h"
#include <util/delay.h>

void initialisationUART(void) {
    // 2400 bauds. Nous vous donnons la valeur des deux
    UBRR0H = 0;
    UBRR0L = 0xCF;

    // permettre la réception et la transmission par le UART0
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    // Format des trames: 8 bits, 1 stop bits, sans parité
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}

void transmissionUART(uint8_t data) {
    //  Attendre que le buffer soit vide
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    // Mettre la donner à transmettre dans le buffer
    UDR0 = data;
}

int main() {
    initialisationUART();

    Memoire24CXXX memory;

    uint8_t adress = FIRST_ADRESS_MEMORY;
    uint8_t buffer;
    do {
        memory.lecture(adress, &buffer);
        transmissionUART(buffer);
        adress++;
    } while (buffer != '\0');
    return 0;
}