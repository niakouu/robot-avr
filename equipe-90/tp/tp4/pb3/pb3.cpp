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

void ajustementPwm (uint16_t duree) {
// mise à un des sorties OC1A et OC1B sur comparaison
// réussie en mode PWM 8 bits, phase correcte
// et valeur de TOP fixe à 0xFF (mode #1 de la table 16-5
// page 130 de la description technique du ATmega324PA)
OCR1A = duree ;
OCR1B = duree ;

// division d'horloge par 8 - implique une fréquence de PWM fixe
TCCR1A |= (1 << WGM10) | (1 << COM1A0) |  (1 << COM1A1) | (1 << COM1B0) |  (1 << COM1B1); //page 128
TCCR1B |= (1 << CS11); //page 131
TCCR1C = 0;
}

int main()
{
    DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);
    

    ajustementPwm(0);
    _delay_ms(2000);
    ajustementPwm(50);
    _delay_ms(2000);
    ajustementPwm(150);
    _delay_ms(2000);
    ajustementPwm(255);
    
    return 0;
}