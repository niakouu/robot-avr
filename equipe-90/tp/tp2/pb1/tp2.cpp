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

enum class Etat
{
    premierAppui,
    deuxiemeAppui,
    troisiemeAppui,
    allumerDel
};

bool debounce()
{
    static bool casPrecedent = false;
    bool casActuel = false;

    if (PIND & (1 << PD2))
    {
        _delay_ms(50);
        if (PIND & (1 << PD2))
        {
            casActuel = true;
        }
    }

    bool frontMontant = (!casPrecedent && casActuel);
    casPrecedent = casActuel;

    return frontMontant;
}

int main()
{
    DDRA |= (11 << DDRA); 
    PORTA &= ~(11 << PORTA);
    DDRD &= ~(11 << DDRD);

    Etat etatCourant = Etat::premierAppui;

    while (true)
    {
        switch (etatCourant)
        {
            case Etat::premierAppui:
            {
                if (debounce())
                {
                    etatCourant = Etat::deuxiemeAppui;
                }
                break;
            }
            case Etat::deuxiemeAppui:
            {
                if (debounce())
                {
                    etatCourant = Etat::troisiemeAppui;
                }
                break;
            }
            case Etat::troisiemeAppui:
            {
                if (debounce())
                {
                    etatCourant = Etat::allumerDel;
                }
                break;
            }
            case Etat::allumerDel:
            {
                PORTA |= (1 << PA0); // allumer la del en vert
                _delay_ms(2000);
                PORTA &= ~(1 << PA0); // eteindre la del
                etatCourant = Etat::premierAppui;
                break;
            }
        }
    }
    return 0;
}
