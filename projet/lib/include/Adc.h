/*
 * Classe permettant l'acces au convertisseur analogique/numerique
 * du microcontroleur ATMega16 de Atmel.
 *
 * Ecole Polytechnique de Montreal
 * Departement de genie informatique
 * Cours inf1995
 *
 * Matthew Khouzam et Jerome Collin
 * 2005-2006
 *
 * Code qui n'est sous aucune license.
 *
 */

#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

/*
 * Classe Adc:
 *   Le constructeur initialise le convertisseur.
 *   Une lecture enclanche une conversion et le resultat
 *   est retourne sur 16 bits.
 *
 */

class Adc {
public:
    Adc(Adc&) = delete;
    void operator=(const Adc&) = delete;

    // retourne la valeur numerique correspondant a la valeur
    // analogique sur le port A.  pos doit etre entre 0 et 7
    // inclusivement.  Seulement les 10 bits de poids faible
    // sont significatifs.
    uint16_t read(const uint8_t pos) const;

protected:
    friend class Board;
    Adc();
    ~Adc();

private:
    static constexpr uint8_t MASK_SEVEN = 0x07;
};

#endif /* ADC_H */
