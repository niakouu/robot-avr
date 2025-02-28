#include <avr/io.h>
#include <util/delay.h>

// void showS() {
//     for (int i = 0; i < 3; ++i) 
//     {
//         PORTA |= (1 << PA1);
//         _delay_ms(5000);
//         PORTA &= ~(1 << PA1);
//         _delay_ms(5000);
//     }
        
// }

// void showO() {
//     for (int i = 0; i < 3; ++i) 
//     {
//         PORTA |= (1 << PA0);
//         _delay_ms(2000);
//         PORTA &= ~(1 << PA0);
//         _delay_ms(2000);
//     }
        
// }

int main() {
    // Configure PORTA as output
    DDRA = 0xFF; // Set all pins on PORTA as output.
    PORTA = 0x00;
    DDRD = 0x00;
    char etat = 'r';
    long compteur = 0;
    //DDRD &= ~(1 <<PD1);
    

    while (1) {

        
        if (PIND & 0x04) {
switch (etat)
        {
        case 'r':
            etat = 'v';
            PORTA = 0xAA;
            break;
        case 'v':
            etat = 'a';
            while (PIND & ~(0x04)) {
            bool vert = false;
            if (vert) {
                PORTA = 0x55;
            }
            else {
                vert = !vert;
                PORTA = 0xAA;
            }
            }
            break;
        case 'a':
            etat = 'r';
            PORTA = 0x55;
            break;            
        default:
            break;
        }


        }
        

        // etat = 'r';
        // PORTA = 0xAA; // Turn all PORTA pins HIGH (LEDs ON if connected to pins).
        // _delay_ms(5000); // Delay for 500 milliseconds.
    
        // PORTA = 0x00; // Turn all PORTA pins LOW (LEDs OFF).
        // _delay_ms(5000); // Delay for 500 milliseconds.

        // etat = 'v';
        // PORTA = 0x55;
        // _delay_ms(5000);

        // PORTA = 0x00; // Turn all PORTA pins LOW (LEDs OFF).
        // _delay_ms(5000); // Delay for 500 milliseconds.


        // etat = 'a';
        // while (compteur <= 100) {
        //     PORTA = 0xAA;
        //     _delay_ms(25);
        //     PORTA = 0x55;
        //     _delay_ms(25);
        //     compteur++;
        // }
        // compteur = 0;

        // PORTA = 0x00; // Turn all PORTA pins LOW (LEDs OFF).
        // _delay_ms(5000); // Delay for 500 milliseconds.

        // showS();
        // showO();
        // showS();
        // _delay_ms(10000);



    }

    return 0;

}

