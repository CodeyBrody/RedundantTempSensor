#include "sensor.h"
#include "faults.h"
#include "main.h"
#include "logging.h"
#include "error_codes.h"
#include "usb_comm.h"
#include "hardware.h"
#include "defines.h"

extern uint8_t TEMP_REGISTER_ADDRESS;
extern float TEMP_CONVERSION_VAL;
extern uint8_t TMP102_ADDRESSES[SENSOR_COUNT];

// /*The actually addresses of each sensor being used (stored in order)*/
uint8_t SENSOR_ADDRESSES[SENSOR_COUNT];

uint8_t I2CBuf[2]; //Buffer to send and receive data to/from the sensors via the I2C bus. 

uint8_t discoverSensorArray(uint8_t SensorCount, Sensor sensors[]){
    HAL_StatusTypeDef ret;
    uint8_t discoveredSensorCount = 0;

    for(int i = 0; i < SensorCount; i++){
        ret = HAL_I2C_Master_Transmit(&hi2c1, TMP102_ADDRESSES[i], I2CBuf, 1, I2C_TIMEOUT_MS);
        if(ret != HAL_OK){ //If we get anything other than HAL_OK. copy error message to buffer and skip next steps...
            continue;
        }
        else{ //If we successfully communicate with a sensor at that address, then add that address to a sensor in the array of sensors
            SENSOR_ADDRESSES[discoveredSensorCount] = TMP102_ADDRESSES[i];
            discoveredSensorCount++;
            if (discoveredSensorCount == SensorCount){
                return discoveredSensorCount;
            }
        }
    }
    //If we get to this point, we did not discover 'SensorCount' number of sensors
    return discoveredSensorCount;
}

void initSensorArray(uint8_t SensorCount, Sensor sensors[]){
    for(int i = 0; i< SensorCount; i++){
        //MODIFY TMP102_ADDRESSES to SENSOR_ADDRESSES if using 'discover sensors' strategy
        Sensor s = {TMP102_ADDRESSES[i], NAN, 0, 0, {REDLeds[i], YELLOWLeds[i], GREENLeds[i]}};
        sensors[i] = s;
    }
    return;
}

void setupSensors(uint8_t SensorCount, Sensor sensors[]){
    /*The following code can be uncommented to discover which sensore are available at the start...
      ...however, if this is done, the sensors that were not discovered at the start will ...
      ...not be retried later in the program should they become available. */
    /*NOTE: See comment in 'initSensorArray' regarding changing the array used to initialize
            the Sensor data structures if you choose to use the 'discover sensors' route.*/

    // int discoveredSensorCount = discoverSensorArray(SensorCount, sensors);
    // if(discoveredSensorCount != SensorCount){
    //     logError(MISSING_SENSORS, discoveredSensorCount);
    // }

    initSensorArray(SensorCount, sensors);
}

float readTempSensor(Sensor *s){
    HAL_StatusTypeDef ret;
    int16_t sensor_value; 
    I2CBuf[0] = TEMP_REGISTER_ADDRESS;
    ret = HAL_I2C_Master_Transmit(&hi2c1, s->address, I2CBuf, 1, I2C_TIMEOUT_MS);
    if(ret != HAL_OK){ //If we get anything other than HAL_OK, retry...
        ret = HAL_I2C_Master_Transmit(&hi2c1, s->address, I2CBuf, 1, I2C_TIMEOUT_MS);
    }
    if(ret != HAL_OK){ //if HAL_OK is still not received, skip the next code...(this will also flow through if the last if statement failed)
      //IF WE LATER WANT TO LOG A MORE SPECIFIC ERROR, CAN INSERT HERE
    } else { //...but if HAL_OKAY was returned, request temp data.
        ret = HAL_I2C_Master_Receive(&hi2c1, s->address, I2CBuf, 2, I2C_TIMEOUT_MS);
        if(ret != HAL_OK){ //If error receiving temp data, retry...
            ret = HAL_I2C_Master_Receive(&hi2c1, s->address, I2CBuf, 2, I2C_TIMEOUT_MS);
        }
        if(ret != HAL_OK){ //if HAL_OK is still not received, skip the next code...(this will also flow through if the last if statement failed)
        //IF WE LATER WANT TO LOG A MORE SPECIFIC ERROR, CAN INSERT HERE
      } else {/*...then calculate the temperature in Celsius from the returned "temperature value".*/

        //Combine the bytes (format 0000|xxxxxxxx|xxxx -> first 8 x's from buf[0], last 4 from buf[1])
        sensor_value = ((uint16_t)I2CBuf[0]<<4 | I2CBuf[1]>>4);

        //Convert to 2's complement, if the temperature is negative (the first bit is 1)
        if(sensor_value > 0x7FF){
          sensor_value |= 0xF000; //Sets the first 4 bits of val to 1, leaves the rest alone (which makes it 2's complement if it was before)
        }

        //Convert to a float temperature value (in degrees Celsius)
        s->currTemp = sensor_value * TEMP_CONVERSION_VAL;
        return 1;
      }
    }

    //If HAL_OK was ever NOT received in any of the if statements above, program execution should proceed to here
    s->faults |= COMM_FAULT;
    s->currTemp = NAN;
    return 0;
}

uint8_t readTempSensors(Sensor sensors[]){
    uint8_t sensorsReadSuccessfully = 0;
    for(int i = 0; i<SENSOR_COUNT; i++){
        if(!readTempSensor(&sensors[i])){
            logError(SENSOR_READ_MISSING, i);
        }
        else{
            sensorsReadSuccessfully++;
        }
    }
    return sensorsReadSuccessfully;
}

void clearFaults(Sensor sensors[]){
    for(int i = 0; i<SENSOR_COUNT; i++){
        sensors[i].lastFaults = sensors[i].faults;
        sensors[i].faults = 0;
    }
    return;
}

