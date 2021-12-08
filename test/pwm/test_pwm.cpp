#include <util/delay.h>
#include <avr/io.h>
#include <MotorController.h>
#include <unity.h>

void pwm_ramp(){
    MotorController::set(0);
    TEST_ASSERT_EQUAL(0, OCR2B);
    _delay_ms(200);
    MotorController::set(63);
    _delay_ms(500);
    MotorController::set(127);
    TEST_ASSERT_EQUAL(124, OCR2B);
    _delay_ms(1000);
    MotorController::set(191);
    _delay_ms(1000);
    MotorController::set(255);
    TEST_ASSERT_EQUAL(250, OCR2B);
    _delay_ms(1000);
    MotorController::set(0);

}

int main(void){
    MotorController::init();
    MotorController::enable();
    _delay_ms(2000);
    UNITY_BEGIN();
    RUN_TEST(pwm_ramp);
    UNITY_END();
    while(1);
}