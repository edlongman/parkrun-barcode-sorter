#include <unity.h>
#include <ServoController.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void test_small_times(){
    ServoController::Timer test(10);
    TEST_ASSERT_FALSE_MESSAGE(test.isExpired(), "Timer should not expire immediately");
    _delay_ms(25);
    TEST_ASSERT_TRUE_MESSAGE(test.isExpired(), "Timer exipired too late");

}

void test_duration(){
    ServoController::Timer test(500);
    // Wait <500 - resolution for trigger
    _delay_ms(475);
    auto const timer_result1 = test.isExpired();
    _delay_ms(25);
    auto const timer_result2 = test.isExpired();
    TEST_ASSERT_FALSE_MESSAGE(timer_result1, "Timer expired too early");
    TEST_ASSERT_TRUE_MESSAGE(timer_result2, "Timer expired too late");
}

void test_very_large_time(){
    ServoController::Timer test(6000);
    // Wait <500 - resolution for trigger
    _delay_ms(3000);
    // Start other timer to test for confilcts
    ServoController::Timer test2(1000);
    //Mid check result
    test.isExpired();
    _delay_ms(2975);
    //Early check result
    auto const timer_result2 = test.isExpired();
    test2.isExpired();
    _delay_ms(25);
    auto const timer_result3 = test.isExpired();
    TEST_ASSERT_FALSE_MESSAGE(timer_result2, "Timer expired too early");
    TEST_ASSERT_TRUE_MESSAGE(timer_result3, "Timer expired too late");
    _delay_ms(1500);
    TEST_ASSERT_TRUE_MESSAGE(test.isExpired(), "Timer didn't stay stopped");
}

void test_restart(){
    TEST_ASSERT_TRUE_MESSAGE(false, "Unimplemented restart test");
}

int main(){
    ServoController::init();
    sei();
    _delay_ms(1500);
    UNITY_BEGIN();
    RUN_TEST(test_duration);
    RUN_TEST(test_small_times);
    RUN_TEST(test_very_large_time);
    RUN_TEST(test_restart);
    UNITY_END();
    while(1);
}