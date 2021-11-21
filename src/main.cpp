#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "FlapGovenor.h"
#include "MotorController.h"
#include "ConsoleUart.h"

int main(void){
    FlapGovenor::init();
    FlapGovenor::enable();
    MotorController::init();
    MotorController::enable();
    ConsoleUart::init();
    char buffer[16] = "Hello World";
    sei();
    while(1){
        ConsoleUart::blockingSendLine(buffer, 16);
        _delay_ms(500);
    }
}