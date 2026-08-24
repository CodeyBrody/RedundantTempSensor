#include "sensor.h"
#include "faults.h"
#include "main.h"
#include "faults.h"

uint8_t TEMP_REGISTER_ADDRESS = 0;
float TEMP_CONVERSION_VAL = 0.0625;

__int8_t TMP102_POSSIBLE_ADDRESSES[] = {0x48, 0x49, 0x4A, 0x4B};
uint8_t TMP102_POSSIBLE_ADDRESS_COUNT = 4;
__int8_t SENSOR_ADDRESSES[SENSOR_COUNT];

__uint8_t discoverSensorArray(__uint8_t SensorCount, Sensor sensors[]){
    HAL_StatusTypeDef ret;
    uint8_t discoveredSensorCount = 0;

    for(int i = 0; i < TMP102_POSSIBLE_ADDRESS_COUNT; i++){
        ret = HAL_I2C_Master_Transmit(&hi2c1, TMP102_POSSIBLE_ADDRESSES[i], txBuf, 1, HAL_MAX_DELAY);
        if(ret != HAL_OK){ //If we get anything other than HAL_OK. copy error message to buffer and skip next steps...
            continue;
        }
        else{ //If we successfully communicate with a sensor at that address, then add that address to a sensor in the array of sensors
            SENSOR_ADDRESSES[discoveredSensorCount] = TMP102_POSSIBLE_ADDRESSES[i];
            if (discoveredSensorCount == SensorCount){
                return discoveredSensorCount;
            }
        }
    }
    //If we get to this point, we did not discover 'SensorCount' number of sensors
    return discoveredSensorCount;
}

void initSensorArray(__uint8_t SensorCount, Sensor sensors[]){
    for(int i = 0; i< SensorCount; i++){
        Sensor s = {SENSOR_ADDRESSES[i], NAN, 0};
        sensors[i] = s;
    }
    return;
}

__uint8_t readTempSensors(Sensor sensors[]){
    uint8_t sensorsReadSuccessfully = 0;
    for(int i = 0; i<SENSOR_COUNT; i++){
        if(!readTempSensor(sensors[i])){
            /*Can decide if want to print error message from txBuf here later.*/
        }
        else{
            sensorsReadSuccessfully++;
        }
    }
    return sensorsReadSuccessfully;
}

float readTempSensor(Sensor s){
    HAL_StatusTypeDef ret;
    int16_t sensor_value; 
    float temp_c = NAN;
    txBuf[0] = TEMP_REGISTER_ADDRESS;
    ret = HAL_I2C_Master_Transmit(&hi2c1, s.address, txBuf, 1, HAL_MAX_DELAY);
    if(ret != HAL_OK){ //If we get anything other than HAL_OK. copy error message to buffer and skip next steps...
      strcpy((char*)txBuf, "Error Tx\r\n");
      s.faults |= COMM_FAULT;
      s.currTemp = NAN;
      return 0;
    } else { //...but if HAL_OKAY was returned, request temp data.
      HAL_I2C_Master_Receive(&hi2c1, s.address, txBuf, 2, HAL_MAX_DELAY);
      if(ret != HAL_OK){ //If error receiving temp data, log a different error and skip next steps..
        strcpy((char*)txBuf, "Error Rx\r\n"); //...replacing potentially garbage data
        s.faults |= COMM_FAULT;
        s.currTemp = NAN;
        return 0;
      } else {/*...then calculate the temperature in Celsius from the returned "temperature value".*/

        //Combine the bytes (format 0000|xxxxxxxx|xxxx -> first 8 x's from buf[0], last 4 from buf[1])
        sensor_value = ((uint16_t)txBuf[0]<<4 | txBuf[1]>>4);

        //Convert to 2's complement, if the temperature is negative (the first bit is 1)
        if(sensor_value > 0x7FF){
          sensor_value |= 0xF000; //Sets the first 4 bits of val to 1, leaves the rest alone (which makes it 2's complement if it was before)
        }

        //Convert to a float temperature value (in degrees Celsius)
        s.currTemp = sensor_value * TEMP_CONVERSION_VAL;
        return 1;
      }
    }
}

void clearFaults(Sensor sensors[]){
    for(int i = 0; i<SENSOR_COUNT; i++){
        sensors[i].faults = 0;
    }
    return;
}

void prepNextSensorRead(__uint8_t SensorCount, Sensor sensors[]){
    clearFaults(sensors);
}
