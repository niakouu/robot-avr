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

void allumerDelAmbre()
{
    allumerDelVert();
    _delay_ms(10);
    allumerDelRouge();

}
int main()
{
    DDRA |= (1 << PA0) | (1 << PA1);
    PORTA &= ~(1 << PA0) & ~(1 << PA1);
    DDRD &= ~(1 << PD0) & ~(1 << PD1);
    Etat etatCourant = Etat::etatInitial;

    while (true)
    {
        switch (etatCourant)
        {
            case Etat::etatInitial:
            {
                allumerDelRouge();
                if (estAppuye())
                {
                    etatCourant = Etat::premierPressement;
                }
                break;
            }
            case Etat::premierPressement:
            {
                allumerDelAmbre();
                if (!estAppuye())
                {
                    etatCourant = Etat::premierRelachement;
                }
                break;
            }
            case Etat::premierRelachement:
            {
                allumerDelVert();
                if (estAppuye())
                {
                    etatCourant = Etat::deuxiemePressement;
                }
                break;
            }
            case Etat::deuxiemePressement:
            {
                allumerDelRouge();
                if (!estAppuye())
                {
                    etatCourant = Etat::deuxiemeRelachement;
                }
                break;
            }
            case Etat::deuxiemeRelachement:
            {
                eteindreDel();
                if (estAppuye())
                {
                    etatCourant = Etat::troisiemePressement;
                }
                break;
            }
            case Etat::troisiemePressement:
            {
                allumerDelVert();
                if (!estAppuye())
                {
                    etatCourant = Etat::etatInitial;
                }
                break;
            }
        }
    }
    return 0;
}