#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <MotorController.h>
#include <WheelGauge.h>
#include "ConsoleUart.h"
#include "ScanInterpreter.h"
#include <stdio.h>

int main(){
    MotorController::init();
    MotorController::enable();
    ConsoleUart::init();
    ConsoleUart::enable();
    WheelGauge::init();
    WheelGauge::enable();
    sei();
    const int8_t buff_size=30;
    char ostream[buff_size];
    _delay_ms(3000);
    sprintf(ostream, " --- Start --- \r\n");
    ConsoleUart::blockingSendLine(ostream, buff_size);
    
    int16_t motorPosition=0;
    int16_t speed=0;
    int16_t integral=0;
    int16_t error=0;
    MotorController::setSpeed(300);
    for(uint8_t i=0;i<50*4;i++){
        MotorController::getPIStateVariables(motorPosition, speed, error, integral);
        sprintf(ostream, "%5i,%5i,%5i,%5i\r\n", motorPosition, speed, error, integral);
        ConsoleUart::blockingSendLine(ostream, buff_size);
        MotorController::waitForPIStep();
    }
    MotorController::setSpeed(0);
    while(1);
}