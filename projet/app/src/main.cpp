//
// Travail : Projet Final
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai 
//
// Description du main.cpp : La fonction de main est d'initialiser
//                                        le UART et d'envoyer des mis à jour à
//                                        des intervals fixes à la classe
//                                        Challenge. Après chaque update, le
//                                        microcontrôler fait un sleep d'une
//                                        même durée de temps.
//
// Identification matérielles :
//      - A0-A4: Les output digitals D0 à D4 du capteur lignes
//      - A5: Le output V0 du capteur de distance
//      - B2: Direction du moteur gauche
//      - B3: Enable du moteur gauche
//      - B4: Enable du moteur droit
//      - B5: Direction du moteur droit
//      - C6: Led négative
//      - C7: Led positive
//      - D3: Le output du bouton-poussoir
//      - D5: Le fils rouge du Midi
//      - D7: Le fils noir du Midi

#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include "Board.h"
#include "Challenge.h"
#include "DistanceSensor.h"
#include "LineFollower.h"
#include "Robot.h"
#include "debug.h"

constexpr const uint16_t BAUD_RATE = 2400;
constexpr const uint16_t UPDATE_DELTA_MS = 25;

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

ISR(INT0_vect) {
    Robot::get().getBoard().getButton().setPressed();
}

ISR(INT1_vect) {
    Robot::get().getExtraButton().setPressed();
}

int main() {
    Uart& uart = Board::get().getUart0();
    uart.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                   Uart::StopBit::ONE_BIT);
    uart.start();
    stdout = uart.getEmulatedFile();
    stdin = uart.getEmulatedFile();

    sei();

    while (true) {
        Challenge::get().update(UPDATE_DELTA_MS);

        Board::get().getWatchdogTimer().sleep(UPDATE_DELTA_MS,
                                              WatchdogTimer::SleepMode::IDLE);
    };

    return 0;
}