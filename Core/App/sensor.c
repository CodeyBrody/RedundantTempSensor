#include "sensor.h"
#include "defines.h"
#include "error_codes.h"
#include "faults.h"
#include "hardware.h"
#include "logging.h"
#include "main.h"
#include "usb_comm.h"

extern uint8_t TEMP_REGISTER_ADDRESS;
extern float TEMP_CONVERSION_VALUE;
extern uint8_t SENSOR_ADDRESSES[SENSOR_COUNT];

uint8_t I2CBuf[2]; // Buffer to send and receive data to/from the sensors via the I2C bus.

uint8_t discoverSensorArray(uint8_t SensorCount)
{
    HAL_StatusTypeDef ret;
    uint8_t discoveredSensorCount = 0; // A variable to be returned holding the count of sensors
                                       // successfully communicated with ('discovered')

    for (int i = 0; i < SensorCount; i++)
    {
        ret = HAL_I2C_Master_Transmit(&hi2c1, SENSOR_ADDRESSES[i], I2CBuf, 1, I2C_TIMEOUT_MS);
        if (ret != HAL_OK)
        { // If we get anything other than HAL_OK, then skip next steps...
            continue;
        }
        else
        { // If we successfully communicate with a sensor at that address...
            discoveredSensorCount++; //...then increment 'discoveredSensorCount'
            if (discoveredSensorCount == SensorCount)
            { // If we have discovered 'SensorCount' sensors, then we're done!
                return discoveredSensorCount; // Return the number of sensors discovered (should be
                                              // SensorCount in this case).
            }
        }
    }
    // If we get to this point, we did not discover 'SensorCount' number of sensors
    return discoveredSensorCount; // Return the number of sensors discovered
}

void initSensorArray(uint8_t SensorCount, Sensor sensors[])
{
    // Initialize structure objects for each of the expected sensors, and place them in `sensors[]`
    for (int i = 0; i < SensorCount; i++)
    {
        Sensor s = {SENSOR_ADDRESSES[i], NAN, 0, 0, {REDLeds[i], YELLOWLeds[i], GREENLeds[i]}};
        sensors[i] = s;
    }
    return;
}

void setupSensors(uint8_t SensorCount, Sensor sensors[])
{

    // Check communication with each expected sensor (using the addresses from SENSOR_ADDRESSES in
    // hardware.c)...
    int discoveredSensorCount = discoverSensorArray(SensorCount);

    if (discoveredSensorCount != SensorCount)
    { // If the number of sensors discovered does not match the anticipated sensor count...
        logError(SENSORS_NOT_DETECTED, discoveredSensorCount); //...log an error
    }

    // Initialize sensor structs (including thosw with addresses not discovered, in case
    // communication with them is established later during operation)
    initSensorArray(SensorCount, sensors);
}

float readTempSensor(Sensor *s)
{
    HAL_StatusTypeDef ret;
    int16_t sensor_value;
    I2CBuf[0] = TEMP_REGISTER_ADDRESS;
    ret = HAL_I2C_Master_Transmit(&hi2c1, s->address, I2CBuf, 1, I2C_TIMEOUT_MS);
    if (ret != HAL_OK)
    { // If we get anything other than HAL_OK, retry...
        ret = HAL_I2C_Master_Transmit(&hi2c1, s->address, I2CBuf, 1, I2C_TIMEOUT_MS);
    }
    if (ret != HAL_OK)
    { // if HAL_OK is not received on either try, skip the next code...
    }
    else
    { //...but if HAL_OKAY was returned, request temp data.
        ret = HAL_I2C_Master_Receive(&hi2c1, s->address, I2CBuf, 2, I2C_TIMEOUT_MS);
        if (ret != HAL_OK)
        { // If error receiving temp data, retry...
            ret = HAL_I2C_Master_Receive(&hi2c1, s->address, I2CBuf, 2, I2C_TIMEOUT_MS);
        }
        if (ret != HAL_OK)
        { // if HAL_OK is still not received, skip the next code...
        }
        else
        { /*...otherwise calculate the temperature in Celsius from the returned "temperature
             value".*/

            // Combine the bytes (ending format 0000|xxxxxxxx|xxxx -> first 8 x's from buf[0], last
            // 4 from buf[1])
            sensor_value = ((uint16_t)I2CBuf[0] << 4 | I2CBuf[1] >> 4);

            // Convert to 2's complement, if the temperature is negative (the first bit is 1)
            if (sensor_value > 0x7FF)
            {
                sensor_value |= 0xF000; // Sets the first 4 bits of val to 1, leaves the rest alone
                                        // (which makes it 2's complement if it was before)
            }

            // Convert to a float temperature value (in degrees Celsius)
            s->currTemp = sensor_value * TEMP_CONVERSION_VALUE;
            return 1;
        }
    }

    // If HAL_OK was ever not received in any of the if statements above, program execution should
    // proceed to here after skipping code above. In this case...
    s->faults |= COMM_FAULT; //...flag a communication fault...
    s->currTemp = NAN;       //...set the current temperature reading for the sensor to NAN...
    return 0;                // and return 0
}

uint8_t readTempSensors(Sensor sensors[])
{
    uint8_t sensorsReadSuccessfully = 0;

    // For each expected sensor...
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        if (!readTempSensor(&sensors[i])) //...attempt to read the sensor
        {
            logError(SENSOR_READ_MISSING, i); // If missing, log an error
        }
        else
        {
            sensorsReadSuccessfully++; // If successful, increment `sensorsReadSuccessfully`
        }
    }
    return sensorsReadSuccessfully; // Return the number of sensors successfully read
}

void clearFaults(Sensor sensors[])
{
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        sensors[i].lastFaults = sensors[i].faults;
        sensors[i].faults = 0;
    }
    return;
}
