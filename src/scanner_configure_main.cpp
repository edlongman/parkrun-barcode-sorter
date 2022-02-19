#include <ConsoleUart.h>
#include <Scanner.h>
#include <FlapGovenor.h>
#include <MotorController.h>
#include <avr/interrupt.h>

const uint8_t max_read = 8;
char token_read[max_read] = {0};

int main(){
    sei();
    Scanner::init();
    ConsoleUart::init();
    FlapGovenor::init();
    Scanner::enable();
    ConsoleUart::enable();
    FlapGovenor::enable();
    MotorController::init();
    MotorController::enable();
    // Enable drivers and scanner
    DDRC |= _BV(PC7);
    PORTC |= _BV(PC7);
    while(1){
        Scanner::scanLine(token_read, max_read);
        Scanner::startScan();
        for(uint16_t i=0;i<1000;i++){
            _delay_ms(1);
            if(Scanner::isScanComplete()){
                break;
            }
        }
        Scanner::endScan();
        if(Scanner::isScanComplete()){
            ConsoleUart::blockingSendLine(token_read, max_read);
        }
        _delay_ms(500);
    }
}