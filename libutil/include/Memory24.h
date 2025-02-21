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

# define F_CPU 8000000UL

#include <avr/io.h>
#include <util/twi.h>

class Memory24CXXX
{
public:

   Memory24CXXX(); // le constructeur appelle init() decrit plus bas
   ~Memory24CXXX();

   // procedure d'initialisation appelee par le constructeur
   // Donc, ne pas le refaire a moins de changements en cours
   // d’utilisation.
   void init();
   
   // la procedure init() initialize a zero le "memory bank". 
   // appeler cette methode uniquement si l'adresse doit changer
   static uint8_t choose_memory_bank(const uint8_t bank);
   
   // deux variantes pour la lecture, celle-ci et la suivante
   // une donnee a la fois
   uint8_t read(const uint16_t address, uint8_t *data);
   // bloc de donnees : longueur doit etre de 127 et moins
   uint8_t read(const uint16_t address, uint8_t *data,
                   const uint8_t length);

   // deux variantes pour la l'ecriture egalement:
   // une donnee a la fois
   uint8_t write(const uint16_t address, const uint8_t data);
   // bloc de donnees : longueur doit etre de 127 et moins
   uint8_t write(const uint16_t address, const uint8_t *data,
                    const uint8_t length);

private:
   // pour l'ecriture
   uint8_t write_page(const uint16_t address, const uint8_t *data,
                       const uint8_t length);

private:
   // donnees membres
   static uint8_t m_peripheral_address;
   const uint8_t PAGE_SIZE;
};

#endif /* MEMOIRE_24_H */
