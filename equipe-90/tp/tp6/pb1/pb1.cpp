/*
Travail : TRAVAIL_PRATIQUE_2
Section # : SECTION 4
Equipe # : EQUIPE_NO_90
Correcteur : Dorine Dantrun

Noms: Ilyes Belhaddad et Mohamed Elyes Bradai
Description : Ce code consiste a faire une interruption sur un bouton externe branche a 
    la breadboard qui met en marche une minuterie. Apres, 12 secondes ou apres avoir relache
     le bouton, il y a un jeu de lumiere au niveau de la DEL et le cycle recommence.

Identifications materielles :
    La DEL est connectee aux broches B0B1 par un cable femelle-femelle
    Le boutton poussoir est l'entree reliee a la broche D2 par un cable male-femelle
    Le VCC/GND du PORTB est connectee au VCC/GND du breadboard par un cable male-femelle
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void initialisation(void)
{

    cli();

    DDRB |= (1 << PB0) | (1 << PB1);
    DDRD &= ~(1 << PD2);

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

    while (true)
    {
        do
        {
        } while ((gMinuterieExpiree != 120 && gBoutonPoussoir == 1) || gMinuterieDemaree == 0);

        for (int i = 0; i < 5; i++)
        {
            allumerDelVert();
            _delay_ms(50);
            eteindreDel();
            _delay_ms(50);
        }

        _delay_ms(2000);

        for (int i = 0; i < gMinuterieExpiree / 2; i++)
        {
            allumerDelRouge();
            _delay_ms(25);
            eteindreDel();
            _delay_ms(25);
        }

        allumerDelVert();
        _delay_ms(1000);
        eteindreDel();

        gBoutonPoussoir = 0;
        gMinuterieDemaree = 0;
    }

    return 0;
}
