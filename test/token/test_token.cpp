#include <util/delay.h>
#include <unity.h>
#include <Token.h>

void test_decoding(){
    Token t1(0,"P0995",2,3);
    TEST_ASSERT_EQUAL_INT16(995, t1.value);
    //Invalid token should come back 0
    Token t2(0,"AAAAA",2,3);
    TEST_ASSERT_EQUAL_INT16(0, t2.value);

    //Must handle largest possible token
    Token t3(0,"P9999",2,3);
    TEST_ASSERT_EQUAL_INT16(9999, t3.value);
}

void test_output_bins(){
    Token t1a(0,"P0010",3,2);
    Token t1b(0,"P0010",3,1);
    Token t1c(0,"P0010",3,0);
    TEST_ASSERT_EQUAL_INT8(1, t1a.output);
    TEST_ASSERT_EQUAL_INT8(0, t1b.output);
    TEST_ASSERT_EQUAL_INT8(1, t1c.output);

    Token t2a(0,"P0011",3,2);
    Token t2b(0,"P0011",3,1);
    Token t2c(0,"P0011",3,0);
    TEST_ASSERT_EQUAL_INT8(1, t2a.output);
    TEST_ASSERT_EQUAL_INT8(0, t2b.output);
    TEST_ASSERT_EQUAL_INT8(2, t2c.output);

    Token t3a(0,"P1000",3,6);
    Token t3b(0,"P1000",3,2);
    Token t3c(0,"P1000",3,0);
    TEST_ASSERT_EQUAL_INT8(1, t3a.output);
    TEST_ASSERT_EQUAL_INT8(0, t3b.output);
    TEST_ASSERT_EQUAL_INT8(1, t3c.output);

    Token t4a(0,"P0011",7,2);
    Token t4b(0,"P0011",7,1);
    Token t4c(0,"P0011",7,0);
    TEST_ASSERT_EQUAL_INT8(0, t4a.output);
    TEST_ASSERT_EQUAL_INT8(1, t4b.output);
    TEST_ASSERT_EQUAL_INT8(4, t4c.output);

    Token t6a(0,"P1000",7,3);
    Token t6b(0,"P1000",7,1);
    Token t6c(0,"P1000",7,0);
    TEST_ASSERT_EQUAL_INT8(2, t6a.output);
    TEST_ASSERT_EQUAL_INT8(2, t6b.output);
    TEST_ASSERT_EQUAL_INT8(6, t6c.output);
}

int main(){
    _delay_ms(2000);
    UNITY_BEGIN();
    RUN_TEST(test_decoding);
    RUN_TEST(test_output_bins);
    UNITY_END();
}