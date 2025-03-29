/******************************************************************************/
/*                                                                            */
/* Isidore Lauzier - 9 juillet 2005                                           */
/*                                                                            */
/* Cet exemple d'utilisation d'une memoire eeprom i2c est une modification du */
/* programme de Joerg Wunsch twitest.c fourni avec Winavr. Il est compatible  */
/* aux eeprom 24C128, 24C256 et 24c512. Ce programme a ete simplifie afin     */
/* d'illustrer la programmation du bus TWI et des memoires I2C.               */
/*                                                                            */
/* NOTE IMPORTANTE: Le protocule I2C utilise les broches C0 et C1 sur un      */
/*                  microcontroleur Atmel ATMega244pa.                        */
/*                                                                            */
/* Modifications mineures:                                                    */
/*   - Pour refaire l'indentation - ete 2007 - Jerome Collin                  */
/*   - Pour preciser les broches utilisees - novembre 2008 - Jerome Collin    */
/*   - ajouts de commentaires - septembre 2017 - Jerome Collin                */
/*   - Correction de certains commentaires - septembre 2022 - Jérôme Collin   */
/*   - Changement des noms de fonctions en anglais - Edelina Alieva           */
/*                                                                            */
/******************************************************************************/

#ifndef MEMOIRE_24_H
#define MEMOIRE_24_H

#include <avr/io.h>
#include <util/twi.h>

class Memory24 {
public:
    enum class Bank : uint8_t { ZERO = 0U, ONE = 1U, TWO = 2U, THREE = 3U };

    Memory24(Memory24&) = delete;
    void operator=(const Memory24&) = delete;

    // procedure d'initialisation appelee par le constructeur
    // Donc, ne pas le refaire a moins de changements en cours
    // d’utilisation.
    void init();

    // la procedure init() initialize a zero le "memory bank".
    // appeler cette methode uniquement si l'adresse doit changer
    uint8_t choose_memory_bank(const Bank bank);

    // deux variantes pour la lecture, celle-ci et la suivante
    // une donnee a la fois
    uint8_t read(const uint16_t address) const;
    // bloc de donnees : longueur doit etre de 127 et moins
    void read(const uint16_t address, uint8_t* data,
              const uint8_t length) const;

    // deux variantes pour la l'ecriture egalement:
    // une donnee a la fois
    void write(const uint16_t address, const uint8_t data) const;
    // bloc de donnees : longueur doit etre de 127 et moins
    void write(const uint16_t address, const uint8_t* data,
               const uint8_t length) const;

protected:
    friend class Board;
    Memory24();
    ~Memory24() = default;

private:
    static constexpr uint8_t PAGE_SIZE = 128U;
    static constexpr uint8_t PERIPHERAL_ADDRESS = 0xA0U;
    static constexpr uint32_t SCL_FREQUENCY = 100000UL;
    static constexpr uint8_t SCL_OFFSET = 16U;
    uint8_t peripheral_address_;

    uint8_t write_page(const uint16_t address, const uint8_t* data,
                       const uint8_t length) const;
};

#endif /* MEMOIRE_24_H */
