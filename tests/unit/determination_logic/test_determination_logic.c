#include "determination_logic.h"
#include "faults.h"
#include "hardware.h"
#include "math.h"
#include "unity.h"

static Sensor sensors[SENSOR_COUNT];
float findAverageValidTemp(Sensor sensors[]);

void setUp(void)
{
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].address = 0;
        sensors[i].currTemp = 0.0f;
        sensors[i].faults = 0;
        sensors[i].lastFaults = 0;
    }
}

void tearDown(void) {}

void test_findAverageValidTemp_allSensorsValid(void)
{
    sensors[0].currTemp = 20.0f;
    sensors[1].currTemp = 21.0f;
    sensors[2].currTemp = 22.0f;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.0f, result);
}

void test_findAverageValidTemp_ignoredFaultySensors(void)
{
    sensors[0].currTemp = 20.0f;
    sensors[1].currTemp = 21.0f;
    sensors[2].currTemp = 100.0f;

    sensors[2].faults = COMM_FAULT;

    float result = findAverageValidTemp(sensors);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.5f, result);
}