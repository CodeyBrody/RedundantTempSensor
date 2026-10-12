#include "sensor.h"
#include "unity.h"

static Sensor sensors[SENSOR_COUNT];
float readTempSensor(Sensor *s);

extern volatile HAL_StatusTypeDef fake_HAL_Transmit_StatusReturn;
extern volatile HAL_StatusTypeDef fake_HAL_Transmit_StatusReturn_2;
extern uint8_t fake_HAL_Transmit_StatusCallCount;

extern volatile HAL_StatusTypeDef fake_HAL_Receive_StatusReturn;
extern volatile HAL_StatusTypeDef fake_HAL_Receive_StatusReturn_2;
extern uint8_t fake_HAL_Receive_StatusCallCount;

extern volatile float fakeTempSensorVals[SENSOR_COUNT];

void readTempSensorSensorSetup(Sensor sensors[SENSOR_COUNT])
{
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].currTemp = 0.0f;
        sensors[i].faults = 0;
        sensors[i].lastFaults = 0;
    }
    sensors[0].address = (0x48 << 1);
    sensors[1].address = (0x49 << 1);
    sensors[2].address = (0x4A << 1);
}

void test_sensor_readsNegativeTempCorrectly(void)
{
    readTempSensorSensorSetup(sensors);

    fakeTempSensorVals[0] = -40;
    fakeTempSensorVals[1] = -2.0;
    fakeTempSensorVals[2] = -120;

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        readTempSensor(&sensors[i]);
    }

    TEST_ASSERT_FLOAT_WITHIN(.0001, fakeTempSensorVals[0], sensors[0].currTemp);
    TEST_ASSERT_FLOAT_WITHIN(.0001, fakeTempSensorVals[1], sensors[1].currTemp);
    TEST_ASSERT_FLOAT_WITHIN(.0001, fakeTempSensorVals[2], sensors[2].currTemp);
}