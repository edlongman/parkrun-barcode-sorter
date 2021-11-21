#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "FlapGovenor.h"
#include "MotorController.h"
#include "ConsoleUart.h"

const uint8_t buff_size = 16;

int main(void){
    FlapGovenor::init();
    FlapGovenor::enable();
    MotorController::init();
    MotorController::enable();
    ConsoleUart::init();
    ConsoleUart::enable();
    char tx_buffer[buff_size] = "Hello World";
    char rx_buffer[buff_size];
    sei();
    ConsoleUart::scanLine(rx_buffer, buff_size);
    while(1){
        ConsoleUart::blockingSendLine(tx_buffer, buff_size);
        _delay_ms(500);
        if(ConsoleUart::isScanComplete()){
            // Copy into tx buffer and setup next scanning
            memcpy(tx_buffer, rx_buffer, buff_size);
            ConsoleUart::scanLine(rx_buffer, buff_size);
        }
    }
}