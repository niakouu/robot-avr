/*
Travail : TRAVAIL_PRATIQUE_2
Section # : SECTION 4
Equipe # : EQUIPE_NO_90
Correcteur : Meriam Ben Rabia

Noms: Ilyes Belhaddad et Mohamed Elyes Bradai
Description : Machine a etat disposant de 6 etats qui fait varier les
couleurs d'une DEL selon les entrees de l'utilisateur sur l'interrupteur.

Identifications materielles :
    Le bouton-poussoir libre pour usage general est l'entree reliee a la broche D2
    La DEL est connectee aux broches A0A1 par un cable femelle-femelle0

+---------------------+--------+---------------------+-----------------+
| État présent        | Entrée | État suivant        | Sortie          |
+---------------------+--------+---------------------+-----------------+
| etatInitial         | 0      | etatInitial         | allumerDelRouge |
| etaitInitial        | 1      | premierPressement   | allumerDelRouge |
| premierPressement   | 0      | premierRelachement  | allumerDelAmbre |
| premierPressement   | 1      | premierPressement   | allumerDelAmbre |
| premierRelachement  | 0      | premierRelachement  | allumerDelVert  |
| premierRelachement  | 1      | deuxiemePressement  | allumerDelVert  |
| deuxiemePressement  | 0      | deuxiemeRelachement | allumerDelRouge |
| deuxiemePressement  | 1      | deuxiemePressement  | allumerDelRouge |
| deuxiemeRelachement | 0      | deuxiemeRelachement | eteindreDel     |
| deuxiemeRelachement | 1      | troisiemePressement | eteindreDel     |
| troisiemePressement | 0      | etatInitial         | allumerDelVert  |
| troisiemePressement | 1      | troisiemePressement | allumerDelVert  |
+---------------------+--------+---------------------+-----------------+
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

enum class Etat
{
    etatInitial,
    premierPressement,
    premierRelachement,
    deuxiemePressement,
    deuxiemeRelachement,
    troisiemePressement
};

bool estAppuye()
{
    if (PIND & (1 << PD2))
    {
        _delay_ms(10);
        if (PIND & (1 << PD2))
        {
            return true;
        }
    }
    return false;
}

void eteindreDel()
{
    PORTA &= ~(1 << PA0);
    PORTA &= ~(1 << PA1);
}

void allumerDelVert()
{
    PORTA &= ~(1 << PA1);
    PORTA |= (1 << PA0);
}

void allumerDelRouge()
{
    PORTA &= ~(1 << PA0);
    PORTA |= (1 << PA1);
}

volatile uint8_t gMinuterieExpiree;
volatile uint8_t gBoutonPoussoir;

ISR(TIMER1_COMPA_vect)
{
    gMinuterieExpiree = 1;
}

ISR(INT0_vect)
{
    gBoutonPoussoir = 1;
    // anti-rebond
    _delay_ms(30);
    //
}

void partirMinuterie(uint16_t duree)
{
    gMinuterieExpiree = 0;
    // mode CTC du timer 1 avec horloge divisée par 1024
    // interruption après la durée spécifiée
    TCNT1 = 0;
    OCR1A = duree;
    TCCR1A |= (1 << COM1A0);
    TCCR1B |= (1 << CS10) | (1 << CS12);
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A);
}

int main()
{
    DDRA |= (1 << PA0) | (1 << PA1);
    PORTA &= ~(1 << PA0) & ~(1 << PA1);
    DDRD &= ~(1 << PD0) & ~(1 << PD1);

    // cette procédure ajuste le registre EIMSK
    // de l’ATmega324PA pour permettre les interruptions externes
    EIMSK |= (1 << INT0);
 
    // il faut sensibiliser les interruptions externes aux
    // changements de niveau du bouton-poussoir
    // en ajustant le registre EICRA
    EICRA |= (1 << ISC00);
    EICRA &= ~(1 << ISC01);

    // sei permet de recevoir à nouveau des interruptions.
    sei();
    
    partirMinuterie(78000);

    do
    {

        allumerDelRouge();
        _delay_ms(100);
        eteindreDel();
        _delay_ms(100);
        // attendre qu'une des deux variables soit modifiée
        // par une ou l'autre des interruptions.
    } while (gMinuterieExpiree == 0 && gBoutonPoussoir == 0);
    // Une interruption s'est produite. Arrêter toute
    // forme d'interruption. Une seule réponse suffit.
    cli();
    // Verifier la réponse
    if (gMinuterieExpiree == 1){
        allumerDelRouge();
    }else{
        allumerDelVert();
    }

    return 0;
}