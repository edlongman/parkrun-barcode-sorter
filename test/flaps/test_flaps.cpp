#include <unity.h>
#include <FlapGovenor.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void quick_switch(){
    FlapGovenor::setOn(0);
    TEST_ASSERT_EQUAL_HEX8(_BV(6),(PORTC & 0xF0));
    _delay_ms(500);
    FlapGovenor::setOff(0);
    TEST_ASSERT_EQUAL_HEX8(0x00,PORTC & 0xF0);
    _delay_ms(500);
    FlapGovenor::setOn(1);
    TEST_ASSERT_EQUAL_HEX8(_BV(4),(PORTC & 0xF0));
    _delay_ms(500);
    FlapGovenor::setOff(1);
    TEST_ASSERT_EQUAL_HEX8(0x00,PORTC & 0xF0);
    _delay_ms(500);
    FlapGovenor::setOn(2);
    TEST_ASSERT_EQUAL_HEX8(_BV(5),(PORTC & 0xF0));
    _delay_ms(500);
    FlapGovenor::setOff(2);
    TEST_ASSERT_EQUAL_HEX8(0x00,PORTC & 0xF0);
}

int main(){
    DDRD = _BV(PD4) | _BV(PD6);
    PORTD = 0;
    FlapGovenor::init();
    _delay_ms(2000);
    sei();
    UNITY_BEGIN();
    RUN_TEST(quick_switch);
    UNITY_END();
    PORTC = 0x00;
    while(1){};

}
