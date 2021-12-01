#include <util/delay.h>
#include <avr/io.h>
#include <MotorController.h>
#include <unity.h>

void pwm_ramp(){
    for(uint8_t i=0;i<=100;i+=10){
        MotorController::set(i);
        _delay_ms(500);
        //TEST_ASSERT_EQUAL(OCR2B,i*(uint16_t)150/100);
    }
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