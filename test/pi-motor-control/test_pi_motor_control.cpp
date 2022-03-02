#include <avr/io.h>
#include <MotorController.h>
#include <WheelGauge.h>
#include <unity.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void pi_setpoints(){
    int16_t raw_distance = WheelGauge::read();
    int16_t error_integral = MotorController::setSpeed(300);
    #ifdef FIT0441_DRIVE
    const int target = 864;
    #else
    const int target = 1008;
    #endif
    TEST_ASSERT_INT_WITHIN(6, target, MotorController::getSetpoint());
    _delay_ms(500);
    int16_t raw_distance1 = WheelGauge::read();
    TEST_ASSERT_INT_WITHIN(50, target, MotorController::getSpeed());
    MotorController::setSpeed(250);
    _delay_ms(1000);
    int16_t raw_distance2 = WheelGauge::read();
    TEST_ASSERT_INT_WITHIN(50, 270, WheelGauge::milliturnsDistance(raw_distance1, raw_distance2));
    MotorController::setSpeed(100);
    _delay_ms(500);
    int16_t raw_distance3 = WheelGauge::read();
    TEST_ASSERT_INT_WITHIN(40, 70, WheelGauge::milliturnsDistance(raw_distance2, raw_distance3));
    MotorController::setSpeed(0);
    _delay_ms(500);
}

void wheel_gauge_wraparound(){
    MotorController::setSpeed(250);
    while(WheelGauge::read() < INT16_MAX/100*98){
        _delay_ms(1);
    }
    MotorController::setSpeed(250);
    _delay_ms(40);
    int16_t raw_distance1 = WheelGauge::read();
    _delay_ms(1000);
    int16_t raw_distance2 = WheelGauge::read();
    TEST_ASSERT_INT_WITHIN(80, 270, WheelGauge::milliturnsDistance(raw_distance1, raw_distance2) );

}

int main(){
    WheelGauge::init();
    WheelGauge::enable();
    MotorController::init();
    MotorController::enable();
    DDRC = 0xFF;
    PORTC = 0x00;
    sei();
    _delay_ms(2000);
    UNITY_BEGIN();
    RUN_TEST(pi_setpoints);
    RUN_TEST(wheel_gauge_wraparound);
    MotorController::setPWM(0);
    UNITY_END();
    while(1);
}