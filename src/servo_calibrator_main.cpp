#include <util/delay.h>
#include <avr/interrupt.h>
#include <ConsoleUart.h>
#include <ServoController.h>
#include <stdio.h>

void test_servo_pos(uint8_t val, char* tx_buffer, uint8_t length){
    ServoController::setPWM(val);
    sprintf(tx_buffer, "%3u\r\n", val);
    ConsoleUart::blockingSendLine(tx_buffer, length);
}

int main(){
    sei();
    ServoController::init();
    ServoController::enable();
    _delay_ms(2000);

    const uint8_t buff_size = 20;
    char tx_buffer[buff_size];

    uint8_t center_estimate = 98;
    while(1){
        for(uint8_t i = 1; i<8;i++){
            test_servo_pos(center_estimate - i, tx_buffer, buff_size);
            _delay_ms(500);
            test_servo_pos(center_estimate + i, tx_buffer, buff_size);
            _delay_ms(500);
        }
    }
}