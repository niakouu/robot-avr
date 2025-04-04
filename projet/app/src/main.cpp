#include <avr/interrupt.h>
#include <stdio.h>

#include "Board.h"
#include "Challenge.h"
#include "DistanceSensor.h"
#include "LineFollower.h"
#include "Robot.h"
#include "debug.h"

constexpr const uint16_t BAUD_RATE = 2400;

static volatile Button EXTRA_BUTTON{Button::Interrupt::I1, false};

ISR(WDT_vect) {
    Board::get().getWatchdogTimer().setSleepDone();
}

ISR(INT0_vect) {
    Board::get().getButton().setPressed();
}

ISR(INT1_vect) {
    EXTRA_BUTTON.setPressed();
}

int main() {
    Uart& uart = Board::get().getUart0();
    uart.configure(::BAUD_RATE, false, Uart::Parity::DISABLED,
                   Uart::StopBit::ONE_BIT);
    uart.start();
    stdout = uart.getEmulatedFile();
    stdin = uart.getEmulatedFile();

    Button& button = Board::get().getButton();
    sei();

    const DistanceSensor ds{Pin::Id::P5};
    LineFollower<uint8_t, TimerPrescalerSynchronous> lf{
        Robot::get().getMovementManager(), Robot::get().getLineSensor(), 0.5f};

    lf.start();
    while (true) {
        if (EXTRA_BUTTON.isEvent() && EXTRA_BUTTON.isPressed()) {
            EXTRA_BUTTON.consumeEvent();

            lf.start();
        }

        if (button.isEvent() && button.isPressed()) {
            button.consumeEvent();
            char value = '?';
            float input = 0.0f;
            printf("s %f\n", lf.speed_);
            printf("p %f\n", lf.PID_KP);
            printf("d %f\n", lf.PID_KD);
            printf("i %f\n", lf.PID_KI);
            while (getchar() != 'S')
                ;
            scanf("%c %f", &value, &input);
            switch (value) {
                case 'p':
                    lf.PID_KP = input;
                    break;
                case 's':
                    lf.speed_ = input;
                    break;
                case 'd':
                    lf.PID_KD = input;
                    break;
                case 'i':
                    lf.PID_KI = input;
                    break;
                default:
                    printf("wtf?? %c\n", value);
            }
            
            printf(" s -> %f\n", lf.speed_);
            printf(" p -> %f\n", lf.PID_KP);
            printf(" d -> %f\n", lf.PID_KD);
            printf(" i -> %f\n", lf.PID_KI);

            lf.start();
        }

        lf.update(100);

        Board::get().getWatchdogTimer().sleep(100,
                                              WatchdogTimer::SleepMode::IDLE);
    };

    // Challenge::get().startChallenge();

    return 0;
}