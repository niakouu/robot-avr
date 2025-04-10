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
constexpr const uint16_t UPDATE_DELTA_MS = 100;

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

ISR(INT0_vect) {
    Board::get().getButton().setPressed();
    Robot::get().getButton().consumeEvent();

}

ISR(INT1_vect) {
    // Robot::get().getExtraButton().consumeEvent();
    Robot::get().getExtraButton().setPressed();
    Robot::get().getExtraButton().consumeEvent();
    printf("ON EST LA\n");
}

void adjustMode() {
    auto& lineFollower = Challenge::get().getLineFollower();
    lineFollower.start(LineFollowerConfiguration {.state = LineFollowerState::FORWARD});

    while (true) {
        if (Robot::get().getExtraButton().isEvent()
            && Robot::get().getExtraButton().isPressed()) {
            Robot::get().getExtraButton().consumeEvent();

            lineFollower.start(LineFollowerConfiguration {.state = LineFollowerState::FORWARD});
        }

        if (Board::get().getButton().isEvent()
            && Board::get().getButton().isPressed()) {
            Board::get().getButton().consumeEvent();
            char value = '?';
            float input = 0.0f;
            printf("s %f\n", lineFollower.speed_);
            printf("p %f\n", lineFollower.PID_KP);
            printf("d %f\n", lineFollower.PID_KD);
            printf("i %f\n", lineFollower.PID_KI);
            while (getchar() != 'S')
                ;
            scanf("%c %f", &value, &input);
            switch (value) {
                case 'p':
                    lineFollower.PID_KP = input;
                    break;
                case 's':
                    lineFollower.speed_ = input;
                    break;
                case 'd':
                    lineFollower.PID_KD = input;
                    break;
                case 'i':
                    lineFollower.PID_KI = input;
                    break;
                default:
                    printf("wtf?? %c\n", value);
                    break;
            }

            printf(" s -> %f\n", lineFollower.speed_);
            printf(" p -> %f\n", lineFollower.PID_KP);
            printf(" d -> %f\n", lineFollower.PID_KD);
            printf(" i -> %f\n", lineFollower.PID_KI);

            lineFollower.start(LineFollowerConfiguration {.state = LineFollowerState::FORWARD});
        }

        lineFollower.update(UPDATE_DELTA_MS);

        Board::get().getWatchdogTimer().sleep(UPDATE_DELTA_MS,
                                              WatchdogTimer::SleepMode::IDLE);
    }
}

int main() {
    Uart& uart = Board::get().getUart0();
    uart.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                   Uart::StopBit::ONE_BIT);
    uart.start();
    stdout = uart.getEmulatedFile();
    stdin = uart.getEmulatedFile();

    sei();

    // adjustMode();

    while (true) {
        // if (Robot::get().getExtraButton().isEvent()
        // && Robot::get().getExtraButton().isPressed()) {
        //         printf("zaaaaaaaaaaa\n");
        //         Robot::get().getExtraButton().consumeEvent();

        // }else{
        //     //printf("zbbbbbbbbbbb\n");
        // }

        Challenge::get().update(UPDATE_DELTA_MS);

        // _delay_ms(UPDATE_DELTA_MS);

        // Board::get().getWatchdogTimer().sleep(UPDATE_DELTA_MS,
        //                                       WatchdogTimer::SleepMode::IDLE);
    };

    return 0;
}