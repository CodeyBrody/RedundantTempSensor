#include "determination_logic.h"
#include "faults.h"
#include "unity.h"

static Sensor sensors[SENSOR_COUNT];
float determineTemp(Sensor sensors[]);

void outlierDetectionSensorSetup(Sensor sensors[SENSOR_COUNT])
{
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].address = 0;
        sensors[i].currTemp = NAN;
        sensors[i].faults = 0;
        sensors[i].lastFaults = 0;
    }
}

/*Tests if 3 non-outliers remain valid*/
void test_outlier_detection_agreeingSensorsRemainValid(void)
{

    outlierDetectionSensorSetup(sensors);

    sensors[0].currTemp = -0.9f;
    sensors[1].currTemp = 0.1f;
    sensors[2].currTemp = 1.1f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 0.1, result);
}

/*Tests each sensor individually marked as an outlier, both above and below the average*/
void test_outlier_detection_marksSensor0OutlierBelowInvalid(void)
{
    outlierDetectionSensorSetup(sensors);

    sensors[0].currTemp = 10.0f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 25.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 24.85, result);
}

void test_outlier_detection_marksSensor0OutlierAboveInvalid(void)
{
    outlierDetectionSensorSetup(sensors);

    sensors[0].currTemp = 28.0f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 25.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[0].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 24.85, result);
}

void test_outlier_detection_marksSensor1OutlierBelowInvalid(void)
{
    outlierDetectionSensorSetup(sensors);

    sensors[0].currTemp = 22.0f;
    sensors[1].currTemp = 10.9f;
    sensors[2].currTemp = 21.9f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 21.95, result);
}

void test_outlier_detection_marksSensor1OutlierAboveInvalid(void)
{
    outlierDetectionSensorSetup(sensors);

    sensors[0].currTemp = 22.0f;
    sensors[1].currTemp = 30.9f;
    sensors[2].currTemp = 21.9f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[1].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 21.95, result);
}

void test_outlier_detection_marksSensor2OutlierBelowInvalid(void)
{
    outlierDetectionSensorSetup(sensors);

    sensors[0].currTemp = 22.0f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 19.8f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 22.95, result);
}

void test_outlier_detection_marksSensor2OutlierAboveInvalid(void)
{
    outlierDetectionSensorSetup(sensors);

    sensors[0].currTemp = 22.0f;
    sensors[1].currTemp = 23.9f;
    sensors[2].currTemp = 36.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_FALSE(IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_BITS(IS_OUTLIER, IS_OUTLIER, sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 22.95, result);
}

/*If all sensors are outliers of each other*/
void test_outlier_detection_marksAllOutliers(void)
{
    outlierDetectionSensorSetup(sensors);

    sensors[0].currTemp = -4.0f;
    sensors[1].currTemp = 0.0f;
    sensors[2].currTemp = 4.0f;

    float result = determineTemp(sensors);

    TEST_ASSERT_TRUE(IS_OUTLIER & sensors[0].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER & sensors[1].faults);
    TEST_ASSERT_TRUE(IS_OUTLIER & sensors[2].faults);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 4.0, result);
}
