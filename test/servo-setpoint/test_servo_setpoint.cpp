#include <unity.h>
#include <ServoController.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void check_frequency(){
    cli();
    uint16_t top_val = OCR1A;
    sei();
    TEST_ASSERT_EQUAL(136, OCR1AL);
    TEST_ASSERT_EQUAL(146, OCR1AH);
    TEST_ASSERT_EQUAL_UINT16(37499U,top_val);
}

void quick_switch(){
    ServoController::setPWM(97);
    TEST_ASSERT_EQUAL_UINT16(97,OCR1B);
    _delay_ms(150);
    ServoController::setPWM(104);
    TEST_ASSERT_EQUAL_UINT16(104,OCR1B);
    _delay_ms(250);
    ServoController::setPWM(90);
    TEST_ASSERT_EQUAL_UINT16(90,OCR1B);
    _delay_ms(150);
}

void setpoint_control(){
    using ServoController::State;
    ServoController::enableStates();
    while(ServoController::getState()!=State::collect){}
    ServoController::setTarget(State::insert);
    while(ServoController::getState()!=State::insert){}
}

int main(){
    ServoController::init();
    sei();
    _delay_ms(2000);
    ServoController::enable();
    UNITY_BEGIN();
    RUN_TEST(quick_switch);
    _delay_ms(2000);
    RUN_TEST(setpoint_control);
    UNITY_END();
    _delay_ms(2000);
    ServoController::disable();
    ServoController::setPWM(0);
}
