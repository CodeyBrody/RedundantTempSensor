#include "determination_logic.h"
#include "faults.h"
#include "unity.h"
#include <math.h>

static Sensor sensors[SENSOR_COUNT];
float determineTemp(Sensor sensors[]);

void missingSensorsSensorSetup(Sensor sensors[SENSOR_COUNT])
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

/*Tests for Expected Results with Sensors Missing*/
void test_missing_sensors_sensor0MissingNoOutliers(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].faults |= COMM_FAULT;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 25.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[0].faults & COMM_FAULT);
    TEST_ASSERT_FALSE(sensors[1].faults & IS_OUTLIER);
    TEST_ASSERT_FALSE(sensors[2].faults & IS_OUTLIER);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 24.85, result);
}

void test_missing_sensors_sensor0MissingOthersOutliers(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].faults |= COMM_FAULT;
    sensors[1].currTemp = 21.8f;
    sensors[2].currTemp = 25.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[0].faults & COMM_FAULT);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.8, result);
}

void test_missing_sensors_sensor0MissingOthersOutliersPermutation2(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].faults |= COMM_FAULT;
    sensors[1].currTemp = 25.8f;
    sensors[2].currTemp = 21.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[0].faults & COMM_FAULT);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 21.8f, result);
}

void test_missing_sensors_sensor1MissingNoOutliers(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].currTemp = 23.9f;
    sensors[1].faults |= COMM_FAULT;
    sensors[2].currTemp = 25.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[1].faults & COMM_FAULT);
    TEST_ASSERT_FALSE(sensors[0].faults & IS_OUTLIER);
    TEST_ASSERT_FALSE(sensors[2].faults & IS_OUTLIER);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 24.85, result);
}

void test_missing_sensors_sensor1MissingOthersOutliers(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].currTemp = 21.8f;
    sensors[1].faults |= COMM_FAULT;
    sensors[2].currTemp = 25.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[1].faults & COMM_FAULT);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.8, result);
}

void test_missing_sensors_sensor1MissingOthersOutliersPermutation2(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].currTemp = 25.8f;
    sensors[1].faults |= COMM_FAULT;
    sensors[2].currTemp = 21.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[1].faults & COMM_FAULT);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 21.8f, result);
}

void test_missing_sensors_sensor2MissingNoOutliers(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].currTemp = 23.9f;
    sensors[1].currTemp = 25.8f;
    sensors[2].faults |= COMM_FAULT;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[2].faults & COMM_FAULT);
    TEST_ASSERT_FALSE(sensors[0].faults & IS_OUTLIER);
    TEST_ASSERT_FALSE(sensors[1].faults & IS_OUTLIER);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 24.85, result);
}

void test_missing_sensors_sensor2MissingOthersOutliers(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].currTemp = 21.8f;
    sensors[1].currTemp = 25.8f;
    sensors[2].faults |= COMM_FAULT;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[2].faults & COMM_FAULT);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 25.8, result);
}

void test_missing_sensors_sensor2MissingOthersOutliersPermutation2(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].currTemp = 25.8f;
    sensors[1].currTemp = 21.8f;
    sensors[2].faults |= COMM_FAULT;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(sensors[2].faults & COMM_FAULT);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 21.8f, result);
}

void test_missing_sensors_onlySensor0remaining(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].currTemp = 23.9f;
    sensors[1].faults |= COMM_FAULT;
    sensors[2].faults |= COMM_FAULT;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9f, result);
}

void test_missing_sensors_onlySensor1remaining(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[1].currTemp = 23.9f;
    sensors[0].faults |= COMM_FAULT;
    sensors[2].faults |= COMM_FAULT;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9f, result);
}

void test_missing_sensors_onlySensor2remaining(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[2].currTemp = 23.9f;
    sensors[0].faults |= COMM_FAULT;
    sensors[1].faults |= COMM_FAULT;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 23.9f, result);
}

void test_missing_sensors_allSensorsMissing(void)
{
    missingSensorsSensorSetup(sensors);

    sensors[0].faults |= COMM_FAULT;
    sensors[1].faults |= COMM_FAULT;
    sensors[2].faults |= COMM_FAULT;

    float result = determineTemp(sensors);

    TEST_ASSERT_BITS(COMM_FAULT, COMM_FAULT, sensors[0].faults);
    TEST_ASSERT_BITS(COMM_FAULT, COMM_FAULT, sensors[1].faults);
    TEST_ASSERT_BITS(COMM_FAULT, COMM_FAULT, sensors[2].faults);
    TEST_ASSERT_FLOAT_IS_NAN(result);
}
