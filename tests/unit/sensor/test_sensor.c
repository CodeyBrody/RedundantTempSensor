#include "error_codes.h"
#include "faults.h"
#include "sensor.h"
#include "unity.h"

static Sensor sensors[SENSOR_COUNT];
uint8_t readTempSensor(Sensor *s);

extern uint8_t fakeMissingSensorAddress[SENSOR_COUNT];

extern volatile HAL_StatusTypeDef fake_HAL_Transmit_StatusReturn;
extern volatile HAL_StatusTypeDef fake_HAL_Transmit_StatusReturn_2;
extern uint8_t fake_HAL_Transmit_CallCount;

extern volatile HAL_StatusTypeDef fake_HAL_Receive_StatusReturn;
extern volatile HAL_StatusTypeDef fake_HAL_Receive_StatusReturn_2;
extern uint8_t fake_HAL_Receive_CallCount;

extern int logErrorCallCount;
extern int lastLoggedError;
extern int lastLoggedOptionalInt;

extern int fake_LogError_Codes[4];
extern int fake_LogError_SensorNums[4];

extern volatile float fakeTempSensorVals[SENSOR_COUNT];

void readTempSensorSensorSetup(Sensor sensors[SENSOR_COUNT])
{
    UnitySetTestFile(__FILE__);

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].currTemp = 0.0f;
        sensors[i].faults = 0;
        sensors[i].lastFaults = 0;

        fakeMissingSensorAddress[i] = 0;
        fakeTempSensorVals[i] = 0.0f;
    }

    sensors[0].address = (0x48 << 1);
    sensors[1].address = (0x49 << 1);
    sensors[2].address = (0x4A << 1);

    fake_HAL_Receive_CallCount = 0;
    fake_HAL_Transmit_CallCount = 0;

    fake_HAL_Transmit_StatusReturn = HAL_OK;
    fake_HAL_Transmit_StatusReturn_2 = HAL_OK;

    fake_HAL_Receive_StatusReturn = HAL_OK;
    fake_HAL_Receive_StatusReturn_2 = HAL_OK;

    logErrorCallCount = 0;
    for (int i = 0; i < 4; i++)
    {
        fake_LogError_Codes[i] = -1;
        fake_LogError_SensorNums[i] = -2;
    }
}

void test_sensor_readsMaxTempCorrectly(void)
{
    readTempSensorSensorSetup(sensors);

    fakeTempSensorVals[0] = 125.0f;
    fakeTempSensorVals[1] = 127.9375f;
    fakeTempSensorVals[2] = 125.0625f;

    uint8_t successfulReads = readTempSensors(sensors);

    TEST_ASSERT_EQUAL_UINT8(3, successfulReads);

    TEST_ASSERT_FLOAT_WITHIN(.0001, 125.0f, sensors[0].currTemp);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 127.9375f, sensors[1].currTemp);
    TEST_ASSERT_FLOAT_WITHIN(.0001, 125.0625f, sensors[2].currTemp);
}

void test_sensor_readsMinTempCorrectly(void)
{
    readTempSensorSensorSetup(sensors);

    fakeTempSensorVals[0] = -40.0f;
    fakeTempSensorVals[1] = -128.0f;
    fakeTempSensorVals[2] = -40.125f;

    uint8_t successfulReads = readTempSensors(sensors);

    TEST_ASSERT_EQUAL_UINT8(3, successfulReads);

    TEST_ASSERT_FLOAT_WITHIN(.0001, -40.0f, sensors[0].currTemp);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -128.0f, sensors[1].currTemp);
    TEST_ASSERT_FLOAT_WITHIN(.0001, -40.125f, sensors[2].currTemp);
}

void test_sensor_sensor0Missing(void)
{
    readTempSensorSensorSetup(sensors);

    fakeMissingSensorAddress[0] = sensors[0].address;

    uint8_t successfulReads = readTempSensors(sensors);

    TEST_ASSERT_EQUAL_UINT8(2, successfulReads);

    TEST_ASSERT_EQUAL_UINT8(COMM_FAULT, sensors[0].faults);
    TEST_ASSERT_FLOAT_IS_NAN(sensors[0].currTemp);

    TEST_ASSERT_EQUAL_UINT8(0, sensors[1].faults);
    TEST_ASSERT_EQUAL_UINT8(0, sensors[2].faults);
}

void test_sensor_sensor1Missing(void)
{
    readTempSensorSensorSetup(sensors);

    fakeMissingSensorAddress[0] = sensors[1].address;

    uint8_t successfulReads = readTempSensors(sensors);

    TEST_ASSERT_EQUAL_UINT8(2, successfulReads);

    TEST_ASSERT_EQUAL_UINT8(COMM_FAULT, sensors[1].faults);
    TEST_ASSERT_FLOAT_IS_NAN(sensors[1].currTemp);

    TEST_ASSERT_EQUAL_UINT8(0, sensors[0].faults);
    TEST_ASSERT_EQUAL_UINT8(0, sensors[2].faults);
}

void test_sensor_sensor2Missing(void)
{
    readTempSensorSensorSetup(sensors);

    fakeMissingSensorAddress[0] = sensors[2].address;

    uint8_t successfulReads = readTempSensors(sensors);

    TEST_ASSERT_EQUAL_UINT8(2, successfulReads);

    TEST_ASSERT_EQUAL_UINT8(COMM_FAULT, sensors[2].faults);
    TEST_ASSERT_FLOAT_IS_NAN(sensors[2].currTemp);

    TEST_ASSERT_EQUAL_UINT8(0, sensors[0].faults);
    TEST_ASSERT_EQUAL_UINT8(0, sensors[1].faults);
}

void test_sensor_retriesFailedTransmit(void)
{
    readTempSensorSensorSetup(sensors);

    fake_HAL_Transmit_StatusReturn = HAL_ERROR;
    fake_HAL_Transmit_StatusReturn_2 = HAL_OK;
    fake_HAL_Receive_StatusReturn = HAL_OK;

    fakeTempSensorVals[0] = 25.0f;

    uint8_t result = readTempSensor(&sensors[0]);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(2, fake_HAL_Transmit_CallCount);
    TEST_ASSERT_EQUAL_UINT8(1, fake_HAL_Receive_CallCount);

    TEST_ASSERT_FLOAT_WITHIN(.0001f, 25.0f, sensors[0].currTemp);
    TEST_ASSERT_EQUAL_UINT8(0, sensors[0].faults);
}

void test_sensor_retriesFailedReceive(void)
{
    readTempSensorSensorSetup(sensors);

    fake_HAL_Transmit_StatusReturn = HAL_OK;
    fake_HAL_Receive_StatusReturn = HAL_ERROR;
    fake_HAL_Receive_StatusReturn_2 = HAL_OK;

    fakeTempSensorVals[0] = 25.0f;

    uint8_t result = readTempSensor(&sensors[0]);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(1, fake_HAL_Transmit_CallCount);
    TEST_ASSERT_EQUAL_UINT8(2, fake_HAL_Receive_CallCount);

    TEST_ASSERT_FLOAT_WITHIN(.0001f, 25.0f, sensors[0].currTemp);
    TEST_ASSERT_EQUAL_UINT8(0, sensors[0].faults);
}

void test_sensor_retriesFailedReceiveAfterRetriedTransmit(void)
{
    readTempSensorSensorSetup(sensors);

    fake_HAL_Transmit_StatusReturn = HAL_ERROR;
    fake_HAL_Transmit_StatusReturn_2 = HAL_OK;
    fake_HAL_Receive_StatusReturn = HAL_ERROR;
    fake_HAL_Receive_StatusReturn_2 = HAL_OK;

    fakeTempSensorVals[0] = 25.0f;

    uint8_t result = readTempSensor(&sensors[0]);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(2, fake_HAL_Transmit_CallCount);
    TEST_ASSERT_EQUAL_UINT8(2, fake_HAL_Receive_CallCount);

    TEST_ASSERT_FLOAT_WITHIN(.0001f, 25.0f, sensors[0].currTemp);
    TEST_ASSERT_EQUAL_UINT8(0, sensors[0].faults);
}

void test_sensor_failsAfterRetriedTransmit(void)
{
    readTempSensorSensorSetup(sensors);

    fake_HAL_Transmit_StatusReturn = HAL_ERROR;
    fake_HAL_Transmit_StatusReturn_2 = HAL_ERROR;
    fake_HAL_Receive_StatusReturn = HAL_OK;

    fakeTempSensorVals[0] = 25.0f;

    uint8_t result = readTempSensor(&sensors[0]);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(2, fake_HAL_Transmit_CallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fake_HAL_Receive_CallCount);

    TEST_ASSERT_FLOAT_IS_NAN(sensors[0].currTemp);
    TEST_ASSERT_EQUAL_UINT8(COMM_FAULT, sensors[0].faults);
}

void test_sensor_failsAfterRetriedReceive(void)
{
    readTempSensorSensorSetup(sensors);

    fake_HAL_Transmit_StatusReturn = HAL_OK;
    fake_HAL_Receive_StatusReturn = HAL_ERROR;
    fake_HAL_Receive_StatusReturn_2 = HAL_ERROR;

    fakeTempSensorVals[0] = 25.0f;

    uint8_t result = readTempSensor(&sensors[0]);

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(1, fake_HAL_Transmit_CallCount);
    TEST_ASSERT_EQUAL_UINT8(2, fake_HAL_Receive_CallCount);

    TEST_ASSERT_FLOAT_IS_NAN(sensors[0].currTemp);
    TEST_ASSERT_EQUAL_UINT8(COMM_FAULT, sensors[0].faults);
}

void test_sensor_doesntRetrySuccessfulTransmitAndReceive(void)
{
    readTempSensorSensorSetup(sensors);

    fake_HAL_Transmit_StatusReturn = HAL_OK;
    fake_HAL_Transmit_StatusReturn_2 = HAL_ERROR;
    fake_HAL_Receive_StatusReturn = HAL_OK;
    fake_HAL_Receive_StatusReturn_2 = HAL_ERROR;

    fakeTempSensorVals[0] = 25.0f;

    uint8_t result = readTempSensor(&sensors[0]);

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_EQUAL_UINT8(1, fake_HAL_Transmit_CallCount);
    TEST_ASSERT_EQUAL_UINT8(1, fake_HAL_Receive_CallCount);

    TEST_ASSERT_FLOAT_WITHIN(.0001f, 25.0f, sensors[0].currTemp);
    TEST_ASSERT_EQUAL_UINT8(0, sensors[0].faults);
}

void test_sensor_allSensorReadsMissingLogsErrors(void)
{
    readTempSensorSensorSetup(sensors);

    fake_HAL_Transmit_StatusReturn = HAL_ERROR;
    fake_HAL_Transmit_StatusReturn_2 = HAL_ERROR;

    uint8_t successfulReads = readTempSensors(sensors);

    TEST_ASSERT_EQUAL_UINT8(0, successfulReads);
    TEST_ASSERT_EQUAL_INT(3, logErrorCallCount);

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        TEST_ASSERT_FLOAT_IS_NAN(sensors[i].currTemp);
        TEST_ASSERT_EQUAL_UINT8(COMM_FAULT, sensors[i].faults);
        TEST_ASSERT_EQUAL_INT(SENSOR_READ_MISSING, fake_LogError_Codes[i]);
        TEST_ASSERT_EQUAL_INT(i, fake_LogError_SensorNums[i]);
    }
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