/*
Travail : TRAVAIL_PRATIQUE_2
Section # : SECTION 4
Equipe # : EQUIPE_NO_90
Correcteur : Meriam Ben Rabia

Noms: Ilyes Belhaddad et Mohamed Elyes Bradai
Description : Machine a etat disposant de 4 etats qui sert de
compteur. Lorsque le bouton est appuye 3 fois, la DEL s'allume en vert
pendant 2 secondes puis s'eteint et le cycle recommence

Identifications materielles :
    Le bouton-poussoir libre pour usage general est l'entree reliee a la broche D2
    La DEL est connectee aux broches A0A1 par un cable femelle-femelle
+----------------+--------+----------------+--------+
| État présent   | Entrée | État suivant   | Sortie |
+----------------+--------+----------------+--------+
| premierAppui   | 0      | premierAppui   | 0      |
| premierAppui   | 1      | deuxiemeAppui  | 0      |
| deuxiemeAppui  | 0      | deuxiemeAppui  | 0      |
| deuxiemeAppui  | 1      | troisiemeAppui | 0      |
| troisiemeAppui | 0      | troisiemeAppui | 0      |
| troisiemeAppui | 1      | allumerDel     | 0      |
| allumerDel     | X      | premierAppui   | 1      |
+----------------+--------+----------------+--------+
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



void initialisation(void)
{

    // cli est une routine qui bloque toutes les interruptions.
    // Il serait bien mauvais d'être interrompu alors que
    // le microcontrôleur n'est pas prêt...
    cli();

    // configurer et choisir les ports pour les entrées
    // et les sorties. DDRx... Initialisez bien vos variables
    DDRA |= (1 << PA0) | (1 << PA1);
    DDRD &= ~(1 << PD0) & ~(1 << PD1);
    // PORTA &= ~(1 << PA0) & ~(1 << PA1);


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
}


enum class Etat
{
    etatInitial,
    premierPressement,
    premierRelachement,
    deuxiemePressement,
    deuxiemeRelachement,
    troisiemePressement
};


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

volatile Etat etatCourant = Etat::etatInitial;; // selon le nom de votre variable

void allumerDelAmbre()
{
    allumerDelVert();
    _delay_ms(10);
    allumerDelRouge();    
    _delay_ms(10);     
}

// placer le bon type de signal d'interruption
// à prendre en charge en argument

ISR(INT0_vect)
{
    // laisser un délai avant de confirmer la réponse du
    // bouton-poussoir: environ 30 ms (anti-rebond)
    _delay_ms(30);

    // se souvenir ici si le bouton est pressé ou relâché

    // changements d'état tels que ceux de la
    // semaine précédente
    switch (etatCourant)
        {
            case Etat::etatInitial:
            {
                etatCourant = Etat::premierPressement;
                break;
            }
            case Etat::premierPressement:
            {
                etatCourant = Etat::premierRelachement;
                break;
            }
            case Etat::premierRelachement:
            {
                etatCourant = Etat::deuxiemePressement;
                break;
            }
            case Etat::deuxiemePressement:
            {
                etatCourant = Etat::deuxiemeRelachement;
                break;
            }
            case Etat::deuxiemeRelachement:
            {
                etatCourant = Etat::troisiemePressement;
                break;
            }
            case Etat::troisiemePressement:
            {
                etatCourant = Etat::etatInitial;
                break;
            }
        }
    
    // Voir la note plus bas pour comprendre cette instruction et son rôle
    EIFR |= (1 << INTF0);
}



int main()
{
    initialisation();

    
    while (true)
    {  
        switch (etatCourant)
        {
            case Etat::etatInitial:
            {
                allumerDelRouge();
                break;
            }
            case Etat::premierPressement:
            {
                allumerDelAmbre();
                break;
            }
            case Etat::premierRelachement:
            {
                allumerDelVert();
                break;
            }
            case Etat::deuxiemePressement:
            {
                allumerDelRouge();
                break;
            }
            case Etat::deuxiemeRelachement:
            {
                eteindreDel();
                break;
            }
            case Etat::troisiemePressement:
            {
                allumerDelVert();
                break;
            }
        }
    }
    return 0;
}
