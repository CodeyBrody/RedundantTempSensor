#include "determination_logic.h"
#include "faults.h"
#include "hardware.h"
#include "math.h"
#include "unity.h"

static Sensor sensors[SENSOR_COUNT];
float findAverageValidTemp(Sensor sensors[]);

void findAverageValidTempSensorSetup(Sensor[SENSOR_COUNT])
{
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].address = 0;
        sensors[i].currTemp = NAN;
        sensors[i].faults = 0;
        sensors[i].lastFaults = 0;
    }
}

void test_findAverageValidTemp_allSensorsValid(void)
{
    findAverageValidTempSensorSetup(sensors);

    sensors[0].currTemp = 20.0f;
    sensors[1].currTemp = 21.0f;
    sensors[2].currTemp = 22.0f;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.0f, result);
}

void test_findAverageValidTemp_ignoresFaultySensor0(void)
{
    findAverageValidTempSensorSetup(sensors);

    sensors[0].currTemp = 121.0f;
    sensors[1].currTemp = 20.0f;
    sensors[2].currTemp = 21.0f;

    sensors[0].faults = COMM_FAULT;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.5f, result);
}

void test_findAverageValidTemp_ignoresFaultySensor1(void)
{
    findAverageValidTempSensorSetup(sensors);

    sensors[0].currTemp = 0.0f;
    sensors[1].currTemp = 100.0f;
    sensors[2].currTemp = -1.0f;

    sensors[1].faults = COMM_FAULT;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, -0.5f, result);
}

void test_findAverageValidTemp_ignoresFaultySensor2(void)
{
    findAverageValidTempSensorSetup(sensors);

    sensors[0].currTemp = -40.0f;
    sensors[1].currTemp = -39.5f;
    sensors[2].currTemp = 21.0f;

    sensors[2].faults = COMM_FAULT;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, -39.75f, result);
}

void test_findAverageValidTemp_onlySensor2Valid(void)
{
    findAverageValidTempSensorSetup(sensors);

    sensors[0].currTemp = 121.0f;
    sensors[1].currTemp = 20.0f;
    sensors[2].currTemp = 21.0f;

    sensors[0].faults = COMM_FAULT;
    sensors[1].faults = COMM_FAULT;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.0f, result);
}

void test_findAverageValidTemp_onlySensor0Valid(void)
{
    findAverageValidTempSensorSetup(sensors);

    sensors[0].currTemp = 121.0f;
    sensors[1].currTemp = 20.0f;
    sensors[2].currTemp = 21.0f;

    sensors[1].faults = COMM_FAULT;
    sensors[2].faults = COMM_FAULT;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 121.0f, result);
}

void test_findAverageValidTemp_onlySensor1Valid(void)
{
    findAverageValidTempSensorSetup(sensors);

    sensors[0].currTemp = 121.0f;
    sensors[1].currTemp = -39.75;
    sensors[2].currTemp = 21.0f;

    sensors[0].faults = COMM_FAULT;
    sensors[2].faults = COMM_FAULT;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, -39.75f, result);
}

void test_findAverageValidTemp_noValidSensorsReturnsNaN(void)
{
    findAverageValidTempSensorSetup(sensors);

    sensors[0].currTemp = 121.0f;
    sensors[1].currTemp = 20.0f;
    sensors[2].currTemp = 21.0f;

    sensors[0].faults = COMM_FAULT;
    sensors[1].faults = COMM_FAULT;
    sensors[2].faults = COMM_FAULT;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_IS_NAN(result);
}