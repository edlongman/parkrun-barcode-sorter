#include <unity.h>
#include <PIController.h>
#include <util/delay.h>
#include <stdint.h>

void pid_step(){
    PIController controller(250);
    controller.setSpeed(300);

    TEST_ASSERT_EQUAL(472, controller.error_integral);

    TEST_ASSERT_EQUAL(255, controller.stepControlEstimation(0));
    TEST_ASSERT_EQUAL(255, controller.stepControlEstimation(60));
    
    auto const control_effort = controller.stepControlEstimation(130);
    TEST_ASSERT_EQUAL(1182, controller.error_integral);
    TEST_ASSERT_EQUAL(246, control_effort);
    TEST_ASSERT_EQUAL(216, controller.stepControlEstimation(190));
    TEST_ASSERT_EQUAL(196, controller.stepControlEstimation(230));
    TEST_ASSERT_EQUAL(171, controller.stepControlEstimation(270));
    TEST_ASSERT_EQUAL(162, controller.stepControlEstimation(285));
}

int main(void){
    _delay_ms(2000);
    UNITY_BEGIN();
    RUN_TEST(pid_step);
    UNITY_END();
    while(1);
}