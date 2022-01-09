#include <unity.h>
#include <ServoController.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void check_frequency(){
    cli();
    uint16_t top_val = OCR1A;
    sei();
    TEST_ASSERT_EQUAL_UINT16(37499U,top_val);
}

void quick_switch(){
    ServoController::setPWM(3000); // 2ms at 12MHz/8
    TEST_ASSERT_EQUAL_UINT16(3000,OCR1B);

    //Wait for new positive pulse
    while(PIND&_BV(PD4));
    while( !(PIND&_BV(PD4)) );
    // Delay just under end to check on
    _delay_us(1998);
    char pwm_op1 = PIND&_BV(PD4);
    _delay_us(3);
    // Just after end check off
    char pwm_op2 = PIND&_BV(PD4);
    TEST_ASSERT_TRUE(pwm_op1);
    TEST_ASSERT_FALSE(pwm_op2);
    _delay_ms(150);
    ServoController::setPWM(3328);
    TEST_ASSERT_EQUAL_UINT16(3328,OCR1B);
    _delay_ms(250);
    ServoController::setPWM(2880);
    TEST_ASSERT_EQUAL_UINT16(2880,OCR1B);
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
    RUN_TEST(check_frequency);
    RUN_TEST(quick_switch);
    _delay_ms(2000);
    RUN_TEST(setpoint_control);
    ServoController::disableStates();
    UNITY_END();
    _delay_ms(2000);
    ServoController::disable();
    ServoController::setPWM(0);
}
