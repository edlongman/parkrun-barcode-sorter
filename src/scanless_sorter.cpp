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
    MotorController::setSpeed(200);
    uint32_t loop_counter = 0; 
    bool insert_token_active = false;
    // Enable drivers and scanner
    DDRC |= _BV(PC7);
    PORTC |= _BV(PC7);
    while(1){
        // Simple loop counting wait for 10000 loops between inserting tokens
        if(insert_token_active == false){
            loop_counter++;
            if(loop_counter>10000L){
                // Exit wait and insert token with token feed
                insert_token_active = true;
                loop_counter = 0;
                ServoController::setTarget(ServoController::State::insert);
            }
        }
        else{
            loop_counter++;
            // Wait until token feed reached insert position
            if(ServoController::getState() == ServoController::State::insert){
                insert_token_active = false;
                loop_counter = 0;
                // Add mock token to token race and return feed to collection mode
                TokenRace::insertToken("P0010");
                ServoController::setTarget(ServoController::State::collect);
            }
        }

        // Flap control module
        for(uint8_t i=0;i<3;i++){
            if(TokenRace::checkFlapOutput(i, WheelGauge::read())){
                FlapGovenor::setOn(i);
            }
            else{
                FlapGovenor::setOff(i);
            }
        }

        // Token race cleanup
        TokenRace::popExpiredTokens(WheelGauge::read());
    }
}