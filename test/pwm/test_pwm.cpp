#include <util/delay.h>
#include <avr/io.h>
#include <MotorController.h>
#include <unity.h>

void pwm_ramp(){
    MotorController::setPWM(0);
    TEST_ASSERT_EQUAL(0, OCR2B);
    _delay_ms(200);
    MotorController::setPWM(63);
    _delay_ms(500);
    MotorController::setPWM(127);
    TEST_ASSERT_EQUAL(124, OCR2B);
    _delay_ms(1000);
    MotorController::setPWM(191);
    _delay_ms(1000);
    MotorController::setPWM(255);
    TEST_ASSERT_EQUAL(250, OCR2B);
    _delay_ms(1000);
    MotorController::setPWM(0);

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