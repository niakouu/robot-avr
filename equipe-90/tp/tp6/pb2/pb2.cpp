/*
Travail : TRAVAIL_PRATIQUE_2
Section # : SECTION 4
Equipe # : EQUIPE_NO_90
Correcteur : Dorine Dantrun

Noms: Ilyes Belhaddad et Mohamed Elyes Bradai
Description : Controler une DEL a l'aide d'une photorésistance. A la lumiere ambiante, la DEL est de
    couleur ambre, lorsque la photorésistance est eclairee par une lampe torche la DEL est de couleur
    rouge. Finalement, en l'absence de lumiere la DEL est de couleur verte. Pour trouver les seuils
    lumineux, nous avons procede par la technique de l'essai-erreur rigoureusement.

Identifications materielles :
    La DEL est connectee aux broches B0B1 par un cable femelle-femelle
    La photorésistance est l'entree reliee a la broche A0 par un cable male-femelle
    Le VCC/GND du PORTB est connectee au VCC/GND du breadboard par un cable male-femelle
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can.h"

void initialisation(void)
{
    cli();

    DDRA &= ~(1 << PA0) & ~(1 << PA1);
    DDRB |= (1 << PB0) | (1 << PB1);

    EIMSK |= (1 << INT0);

    EICRA |= (1 << ISC00);
    EICRA &= ~(1 << ISC01);

    sei();
}

void eteindreDel()
{
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);
}

void allumerDelVert()
{
    PORTB &= ~(1 << PB0);
    PORTB |= (1 << PB1);
}

void allumerDelRouge()
{
    PORTB &= ~(1 << PB1);
    PORTB |= (1 << PB0);
}

void allumerDelAmbre()
{
    allumerDelVert();
    _delay_ms(10);
    allumerDelRouge();
    _delay_ms(10);
}

volatile uint8_t gMinuterieExpiree = 0;
volatile uint8_t gBoutonPoussoir = 0;
volatile uint8_t gMinuterieDemaree = 0;

ISR(TIMER1_COMPA_vect)
{
    gMinuterieExpiree += 1;
}

void partirMinuterie(uint16_t duree)
{
    TCNT1 = 0;
    OCR1A = duree;
    TCCR1A = 0;
    TCCR1B |= (1 << CS10) | (1 << CS12) | (1 << WGM12);
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A);
}

ISR(INT0_vect)
{
    _delay_ms(50);
    if (!(PIND & (1 << PD2)))
    {
        gBoutonPoussoir = 1;
    }

    if (PIND & (1 << PD2))
    {
        if (gMinuterieDemaree)
        {
            gBoutonPoussoir = 0;
        }
    }

    if (!gMinuterieDemaree)
    {
        gMinuterieExpiree = 0;
        gMinuterieDemaree = 1;
        partirMinuterie(782);
    }
}

int main()
{
    initialisation();
    can convertisseur = can();

    uint16_t valeur;

    while (true)
    {

        valeur = convertisseur.lecture(PA0) >> 2;
        if (valeur > 125 && valeur < 240)
        {
            allumerDelAmbre();
        }
        else if (valeur > 240)
        {
            allumerDelRouge();
        }
        else if (valeur < 125)
        {
            allumerDelVert();
        }
    }

    return 0;
}
