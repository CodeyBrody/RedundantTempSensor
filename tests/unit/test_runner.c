#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_findAverageValidTemp_allSensorsValid(void);
void test_findAverageValidTemp_ignoredFaultySensors(void);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_findAverageValidTemp_allSensorsValid);
    RUN_TEST(test_findAverageValidTemp_ignoredFaultySensors);

    return UNITY_END();
}