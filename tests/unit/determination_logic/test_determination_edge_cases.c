#include "determination_logic.h"
#include "faults.h"
#include "unity.h"

/*Test Cases with Particularly Pathological/Nefarious Potential */

static Sensor sensors[SENSOR_COUNT];
float determineTemp(Sensor sensors[]);

void errorGuessingSensorSetup(Sensor sensors[SENSOR_COUNT])
{
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].address = 0;
        sensors[i].currTemp = NAN;
        sensors[i].faults = 0;
        sensors[i].lastFaults = 0;
    }
}

// All 0 temps
void test_error_guesses_all0temps(void)
{

    errorGuessingSensorSetup(sensors);

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].currTemp = 0;
    }

    float results = determineTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.0f, results);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.0f, sensors[0].currTemp);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.0f, sensors[1].currTemp);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.0f, sensors[2].currTemp);
}

// One maximum, one minumum, one in the middle
void test_error_guesses_bothBoundsAndMiddle(void)
{

    errorGuessingSensorSetup(sensors);

    sensors[0].currTemp = 125.0f;
    sensors[1].currTemp = -40.0f;
    sensors[2].currTemp = 0.0f;

    float results = determineTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.0f, results);
    TEST_ASSERT_EQUAL_UINT8(IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_EQUAL_UINT8(IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_EQUAL_UINT8(IS_OUTLIER, sensors[2].faults);
}