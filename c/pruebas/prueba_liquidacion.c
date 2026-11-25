#include "unity.h"

#include "liquidacion.h"

void setUp(void) {}
void tearDown(void) {}

static void test_sin_movimientos(void)
{
    long neto = 1;
    unsigned short com = 1;

    TEST_ASSERT_EQUAL_INT(0, liquidar(0, 0, &neto, &com, 0));
    TEST_ASSERT_EQUAL_INT32(0, neto);
    TEST_ASSERT_EQUAL_UINT16(0, com);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_sin_movimientos);
    return UNITY_END();
}
