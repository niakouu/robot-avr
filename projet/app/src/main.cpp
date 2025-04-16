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
        // printf("d:%d:%d\n", Robot::get().getDistanceSensor().getDistanceCm(), Board::get().getAdc().read(static_cast<uint8_t>(Pin::Id::P7)));
        // if (Robot::get().getExtraButton().isEvent()
        //     && Robot::get().getExtraButton().isPressed()) {
        //     Robot::get().getExtraButton().consumeEvent();

        //     Challenge::get().getLineFollower().start(LineFollowerConfiguration {.state = LineFollowerState::LOST});
        //     Challenge::get().setState(Challenge::State::MAZE_CHALLENGE);
        // }

        // puts("aa");

        Challenge::get().update(UPDATE_DELTA_MS);

        // if (Robot::get().getExtraButton().isEvent()) {
        //     printf("p? %d\n", Robot::get().getExtraButton().isPressed());

        //     Robot::get().getExtraButton().consumeEvent();
        // }

        Board::get().getWatchdogTimer().sleep(UPDATE_DELTA_MS,
                                               WatchdogTimer::SleepMode::IDLE);
    };

    return 0;
}