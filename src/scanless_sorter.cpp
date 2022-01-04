#include <util/delay.h>
#include <avr/interrupt.h>
#include <ConsoleUart.h>
#include <FlapGovenor.h>
#include <MotorController.h>
#include <ServoController.h>
#include <TokenRace.h>
#include <WheelGauge.h>
#include <stdio.h>

int main(){
    sei();
    FlapGovenor::init();
    FlapGovenor::enable();
    MotorController::init();
    MotorController::enable();
    ServoController::init();
    ServoController::enable();
    ServoController::enableStates();
    ConsoleUart::init();
    ConsoleUart::enable();
    WheelGauge::init();
    WheelGauge::enable();
    MotorController::setSpeed(350);
    uint32_t loop_counter = 0; 
    bool insert_token_active = false;
    DDRB |= _BV(PB7);
    PORTB &= ~_BV(PB7);
    while(1){
        if(insert_token_active == false){
            loop_counter++;
            //PORTB |= _BV(PB7);
            if(loop_counter>10000L){
                insert_token_active = true;
                loop_counter = 0;
                ServoController::setTarget(ServoController::State::insert);
            }
        }
        else{
            //PORTB &= ~_BV(PB7);
            loop_counter++;
            if(ServoController::getState() == ServoController::State::insert){
                insert_token_active = false;
                loop_counter = 0;
                TokenRace::insertToken("P0010");
                ServoController::setTarget(ServoController::State::collect);
            }
        }
        for(uint8_t i=0;i<3;i++){
            if(TokenRace::checkFlapOutput(i, WheelGauge::read())){
                FlapGovenor::setOn(i);
            }
            else{
                FlapGovenor::setOff(i);
            }
        }
        TokenRace::popExpiredTokens(WheelGauge::read());
    }
}