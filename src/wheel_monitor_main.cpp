#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <MotorController.h>
#include <WheelGauge.h>
#include "ConsoleUart.h"
#include "ScanInterpreter.h"
#include <stdio.h>

const uint8_t buff_size = 16;

int main(void){
    MotorController::init();
    MotorController::enable();
    ConsoleUart::init();
    ConsoleUart::enable();
    WheelGauge::init();
    WheelGauge::enable();
    char tx_buffer[buff_size] = "Hello World";
    const char set_response[] = "SET MOTOR PWM";
    char rx_buffer[buff_size];
    sei();
    ConsoleUart::scanLine(rx_buffer, buff_size);
    int16_t base_distance = 0;
    int16_t prev_position = 0;
    while(1){
        int16_t new_position = WheelGauge::read();
        int16_t speed = (new_position - prev_position)*2;
        sprintf(tx_buffer, "%limT", (int32_t)(WheelGauge::milliturnsScaling()*(float)(new_position-base_distance)));
        //sprintf(tx_buffer, "%imT", (new_position-base_distance));
        //sprintf(tx_buffer, "state %x", WheelGauge::encoderState());
        prev_position = new_position;
        ConsoleUart::blockingSendLine(tx_buffer, buff_size);
        _delay_ms(500);
        if(ConsoleUart::isScanComplete()){
            // set motor speed
            uint8_t setpoint = 0;
            if(ScanInterpreter::readSetInteger(setpoint, rx_buffer, buff_size)){
                MotorController::setPWM(setpoint);
                ConsoleUart::blockingSendLine(set_response, 15);
            }
            else if(ScanInterpreter::readResetDistance(base_distance, rx_buffer, buff_size)){
                base_distance /= WheelGauge::milliturnsScaling();
            }
            else{
                // Copy into tx buffer and setup next scanning
                memcpy(tx_buffer, rx_buffer, buff_size);
            }
            ConsoleUart::scanLine(rx_buffer, buff_size);
        }
    }
}