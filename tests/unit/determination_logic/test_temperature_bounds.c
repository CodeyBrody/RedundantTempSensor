#include "determination_logic.h"
#include "faults.h"
#include "unity.h"

static Sensor sensors[SENSOR_COUNT];
float determineTemp(Sensor sensors[]);

void temperatureBoundsSensorSetup(Sensor sensors[SENSOR_COUNT])
{
    UnitySetTestFile(__FILE__);

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].address = 0;
        sensors[i].currTemp = NAN;
        sensors[i].faults = 0;
        sensors[i].lastFaults = 0;
    }
}

/*UPPER BOUNDARY TESTS ----------------------------------------*/

void test_aboveOOB_sensor0AboveOthersInBoundsAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 125.1f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 27.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.85, result);
}

void test_aboveOOB_sensor1AboveOthersInBoundsAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 23.9f;
    sensors[1].currTemp = 125.1f;
    sensors[2].currTemp = 27.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.85, result);
}

void test_aboveOOB_sensor2AboveOthersInBoundsAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 27.8;
    sensors[1].currTemp = 23.9;
    sensors[2].currTemp = 125.1;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[2].faults);
    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.85, result);
}

void test_aboveOOB_sensor0AboveOthersInBoundsDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 125.0625f;
    sensors[1].currTemp = 23.875f;
    sensors[2].currTemp = 27.875f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 27.875f, result);
}

void test_aboveOOB_sensor1AboveOthersInBoundsDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 23.875f;
    sensors[1].currTemp = 125.0625f;
    sensors[2].currTemp = 27.875f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL_UINT8((ABOVE_BOUNDS | IS_OUTLIER), sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 27.875f, result);
}

void test_aboveOOB_sensor2AboveOthersInBoundsDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 27.9;
    sensors[1].currTemp = 23.9;
    sensors[2].currTemp = 125.1;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9, result);
}

void test_aboveOOB_sensor0InBoundsOthersAboveAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 23.9f;
    sensors[1].currTemp = 126.0f;
    sensors[2].currTemp = 127.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[1].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 126.5, result);
}

void test_aboveOOB_sensor0InBoundsOthersAboveAllAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 124.9f;
    sensors[1].currTemp = 126.0f;
    sensors[2].currTemp = 127.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[1].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[2].faults);
    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.9f, result);
}

void test_aboveOOB_sensor0InBoundsOthersAboveDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 23.9f;
    sensors[1].currTemp = 126.0f;
    sensors[2].currTemp = 130.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[1].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9f, result);
}

void test_aboveOOB_sensor1InBoundsOthersAboveAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 127.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[2].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 126.5, result);
}

void test_aboveOOB_sensor1InBoundsOthersAboveAllAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = 124.9f;
    sensors[2].currTemp = 127.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[2].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.9f, result);
}

void test_aboveOOB_sensor1InBoundsOthersAboveDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 130.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[0].faults && IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[2].faults && IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9f, result);
}

void test_aboveOOB_sensor2InBoundsOthersAboveAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = 127.0f;
    sensors[2].currTemp = 23.9f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[0].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 126.5, result);
}

void test_aboveOOB_sensor2InBoundsOthersAboveAllAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = 127.0f;
    sensors[2].currTemp = 124.9f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[0].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.9f, result);
}

void test_aboveOOB_sensor2InBoundsOthersAboveDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = 130.0f;
    sensors[2].currTemp = 23.9f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[0].faults && IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS & sensors[1].faults && IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9f, result);
}

void test_aboveOOB_allSensorsAboveAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.5f;
    sensors[1].currTemp = 127.5f;
    sensors[2].currTemp = 128.5f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[0].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[1].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 127.5f, result);
}

void test_aboveOOB_allSensorsAboveDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 125.1f;
    sensors[1].currTemp = 129.1f;
    sensors[2].currTemp = 133.1f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE((ABOVE_BOUNDS | IS_OUTLIER) == sensors[0].faults);
    TEST_ASSERT_TRUE((ABOVE_BOUNDS | IS_OUTLIER) == sensors[1].faults);
    TEST_ASSERT_TRUE((ABOVE_BOUNDS | IS_OUTLIER) == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 133.1f, result);
}

void test_aboveOOB_testUpperBoundary(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 125.0f;
    sensors[1].currTemp = 125.1f;
    sensors[2].currTemp = 124.9f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.95f, result);
}

void test_aboveOOB_sensor0InBoundsOthersAboveOneOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 124.0f;
    sensors[1].currTemp = 130.0f;
    sensors[2].currTemp = 126.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_TRUE((ABOVE_BOUNDS | IS_OUTLIER) == sensors[1].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.0f, result);
}

void test_aboveOOB_sensor1InBoundsOthersAboveOneOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = 124.0f;
    sensors[2].currTemp = 130.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_TRUE((ABOVE_BOUNDS | IS_OUTLIER) == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.0f, result);
}

void test_aboveOOB_sensor2InBoundsOthersAboveOneOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 130.0f;
    sensors[1].currTemp = 126.0f;
    sensors[2].currTemp = 124.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE((ABOVE_BOUNDS | IS_OUTLIER) == sensors[0].faults);
    TEST_ASSERT_TRUE(ABOVE_BOUNDS == sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.0f, result);
}

/*LOWER BOUNDARY TESTS ----------------------------------------*/

void test_belowOOB_sensor0BelowOthersInBoundsAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -40.1f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 27.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.85, result);
}

void test_belowOOB_sensor1BelowOthersInBoundsAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 23.9f;
    sensors[1].currTemp = -40.1f;
    sensors[2].currTemp = 27.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.85, result);
}

void test_belowOOB_sensor2BelowOthersInBoundsAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 27.8;
    sensors[1].currTemp = 23.9;
    sensors[2].currTemp = -40.1;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[2].faults);
    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.85, result);
}

void test_belowOOB_sensor0BelowOthersInBoundsDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -40.1;
    sensors[1].currTemp = 23.9;
    sensors[2].currTemp = 27.9;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 27.9, result);
}

void test_belowOOB_sensor1BelowOthersInBoundsDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 23.9;
    sensors[1].currTemp = -40.1;
    sensors[2].currTemp = 27.9;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 27.9, result);
}

void test_belowOOB_sensor2BelowOthersInBoundsDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 27.9;
    sensors[1].currTemp = 23.9;
    sensors[2].currTemp = -40.1;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9, result);
}

void test_belowOOB_sensor0InBoundsOthersBelowAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 23.9f;
    sensors[1].currTemp = -41.0f;
    sensors[2].currTemp = -42.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[1].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -41.5, result);
}

void test_belowOOB_sensor0InBoundsOthersBelowAllAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -39.9f;
    sensors[1].currTemp = -41.0f;
    sensors[2].currTemp = -42.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[1].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[2].faults);
    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.9f, result);
}

void test_belowOOB_sensor0InBoundsOthersBelowDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 0.0f;
    sensors[1].currTemp = -41.0f;
    sensors[2].currTemp = -45.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[1].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_EQUAL(IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.0f, result);
}

void test_belowOOB_sensor1InBoundsOthersBelowAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.0f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = -42.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[2].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -41.5, result);
}

void test_belowOOB_sensor1InBoundsOthersBelowAllAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.0f;
    sensors[1].currTemp = -39.9f;
    sensors[2].currTemp = -42.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[2].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.9f, result);
}

void test_belowOOB_sensor1InBoundsOthersBelowDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.0f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = -45.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[0].faults && IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[2].faults && IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9f, result);
}

void test_belowOOB_sensor2InBoundsOthersBelowAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.0f;
    sensors[1].currTemp = -42.0f;
    sensors[2].currTemp = -1.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[0].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -41.5, result);
}

void test_belowOOB_sensor2InBoundsOthersBelowAllAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.0f;
    sensors[1].currTemp = -42.0f;
    sensors[2].currTemp = -39.9f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[0].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.9f, result);
}

void test_belowOOB_sensor2InBoundsOthersBelowDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.0f;
    sensors[1].currTemp = -45.0f;
    sensors[2].currTemp = -0.1f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[0].faults && IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS & sensors[1].faults && IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -0.1f, result);
}

void test_belowOOB_allSensorsBelowAgreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.5f;
    sensors[1].currTemp = -42.5f;
    sensors[2].currTemp = -43.5f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[0].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[1].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -42.5f, result);
}

void test_belowOOB_allSensorsBelowDisagreeing(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -40.125f;
    sensors[1].currTemp = -44.125f;
    sensors[2].currTemp = -48.125f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE((BELOW_BOUNDS | IS_OUTLIER) == sensors[0].faults);
    TEST_ASSERT_TRUE((BELOW_BOUNDS | IS_OUTLIER) == sensors[1].faults);
    TEST_ASSERT_TRUE((BELOW_BOUNDS | IS_OUTLIER) == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -48.125f, result);
}

void test_belowOOB_testLowerBoundary(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -40.1f;
    sensors[1].currTemp = -40.0f;
    sensors[2].currTemp = -39.9f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.95f, result);
}

void test_belowOOB_sensor0InBoundsOthersBelowOneOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -39.0f;
    sensors[1].currTemp = -45.0f;
    sensors[2].currTemp = -41.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_TRUE((BELOW_BOUNDS | IS_OUTLIER) == sensors[1].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.0f, result);
}

void test_belowOOB_sensor1InBoundsOthersBelowOneOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.0f;
    sensors[1].currTemp = -39.0f;
    sensors[2].currTemp = -45.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_TRUE((BELOW_BOUNDS | IS_OUTLIER) == sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.0f, result);
}

void test_belowOOB_sensor2InBoundsOthersBelowOneOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -45.0f;
    sensors[1].currTemp = -41.0f;
    sensors[2].currTemp = -39.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE((BELOW_BOUNDS | IS_OUTLIER) == sensors[0].faults);
    TEST_ASSERT_TRUE(BELOW_BOUNDS == sensors[1].faults);
    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.0f, result);
}

/*ABOVE-BELOW COMBINATION TESTS*/

void test_abv_bel_sensor0AboveSensor1BelowSensor2inBounds(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 125.1f;
    sensors[1].currTemp = -41.0f;
    sensors[2].currTemp = 0.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[0].faults);
    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[1].faults);
    TEST_ASSERT_EQUAL(IS_OUTLIER, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.0f, result);
}

void test_abv_bel_sensor0AboveSensor2BelowSensor1inBounds(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = 0.0f;
    sensors[2].currTemp = -40.1f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[0].faults);
    TEST_ASSERT_EQUAL(IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.0f, result);
}

void test_abv_bel_sensor1AboveSensor0BelowSensor2inBounds(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -42.0f;
    sensors[1].currTemp = 128.0f;
    sensors[2].currTemp = 1.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[0].faults);
    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[1].faults);
    TEST_ASSERT_EQUAL(IS_OUTLIER, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 1.0f, result);
}

void test_abv_bel_sensor1AboveSensor2BelowSensor0inBounds(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 1.0f;
    sensors[1].currTemp = 127.0f;
    sensors[2].currTemp = -43.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL(IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[1].faults);
    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 1.0f, result);
}

void test_abv_bel_sensor2AboveSensor0BelowSensor1inBounds(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.3f;
    sensors[1].currTemp = -1.0f;
    sensors[2].currTemp = 125.2f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[0].faults);
    TEST_ASSERT_EQUAL(IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -1.0f, result);
}

void test_abv_bel_sensor2AboveSensor1BelowSensor0inBounds(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 100.0f;
    sensors[1].currTemp = -47.0f;
    sensors[2].currTemp = 140.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL(IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[1].faults);
    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 100.0f, result);
}

void test_abv_bel_sensor0inBoundsSensor1BelowSensor2AboveOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -39.5f;
    sensors[1].currTemp = -41.5f;
    sensors[2].currTemp = 140.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_EQUAL((BELOW_BOUNDS), sensors[1].faults);
    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.5f, result);
}

void test_abv_bel_sensor0inBoundsSensor2AboveSensor1BelowOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 125.0f;
    sensors[1].currTemp = -47.0f;
    sensors[2].currTemp = 126.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[1].faults);
    TEST_ASSERT_EQUAL((ABOVE_BOUNDS), sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 125.0f, result);
}

void test_abv_bel_sensor1inBoundsSensor0BelowSensor2AboveOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -41.0f;
    sensors[1].currTemp = -40.0f;
    sensors[2].currTemp = 140.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL(BELOW_BOUNDS, sensors[0].faults);
    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -40.0f, result);
}

void test_abv_bel_sensor1inBoundsSensor2AboveSensor0BelowOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = -40.5f;
    sensors[1].currTemp = 124.0f;
    sensors[2].currTemp = 126.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[0].faults);
    TEST_ASSERT_EQUAL(0, sensors[1].faults);
    TEST_ASSERT_EQUAL((ABOVE_BOUNDS), sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.0f, result);
}

void test_abv_bel_sensor2inBoundsSensor1BelowSensor0AboveOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = -41.0f;
    sensors[2].currTemp = -39.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL((ABOVE_BOUNDS | IS_OUTLIER), sensors[0].faults);
    TEST_ASSERT_EQUAL((BELOW_BOUNDS), sensors[1].faults);
    TEST_ASSERT_EQUAL(0, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -39.0f, result);
}

void test_abv_bel_sensor2inBoundsSensor0AboveSensor1BelowOutlier(void)
{
    temperatureBoundsSensorSetup(sensors);

    sensors[0].currTemp = 126.0f;
    sensors[1].currTemp = -41.0f;
    sensors[2].currTemp = 124.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_EQUAL((ABOVE_BOUNDS), sensors[0].faults);
    TEST_ASSERT_EQUAL((BELOW_BOUNDS | IS_OUTLIER), sensors[1].faults);
    TEST_ASSERT_EQUAL(0, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 124.0f, result);
}