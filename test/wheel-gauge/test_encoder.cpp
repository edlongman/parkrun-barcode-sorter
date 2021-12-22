#include "WheelGauge.h"
#include <unity.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void test_encoder_output(){
    // Expected rate for 1Hz motor of 1ms increments
    int16_t prev_position = WheelGauge::read();
    for(uint8_t i=0;i<200;i++){
        int16_t new_position = WheelGauge::read();
        if(new_position == prev_position){
            _delay_us(2000);
        }
        else{
            int16_t difference = new_position-prev_position;
            TEST_ASSERT_GREATER_THAN_MESSAGE(0, difference, "Direction is wrong on encoder output");
            TEST_ASSERT_INT_WITHIN_MESSAGE(1, 3 , difference, "Encoder output not correct magnitude");
            UnityPrint("In 100us, Encoder moved approximately ");
            UnityPrintNumber(difference);
            prev_position = new_position;
            break;
        }
    }
    _delay_us(1000);
    
}

int main(void){
    DDRD |= _BV(6);
    WheelGauge::init();
    WheelGauge::enable();
    sei();
    _delay_ms(2000);
    PORTD |= _BV(6);
    _delay_ms(40);
    UNITY_BEGIN();
    RUN_TEST(test_encoder_output);
    UNITY_END();
    PORTD &= ~_BV(6);
    while(1);
}