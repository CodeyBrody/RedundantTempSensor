#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "sensor.h"

__uint8_t discoverSensorArray(__uint8_t SensorCount, Sensor sensors[]);
__uint8_t initSensorArray(Sensor sensors[]);
__uint8_t readTempSensors(Sensor sensors[]);
float determineTemp(Sensor sensors[]);
void setAlerts(Sensor sensors[]);
__uint8_t logData(float displayTemp, Sensor sensors[]);
void clearFaults(Sensor sensors[]);

__uint8_t discoverSensorArray(__uint8_t SensorCount, Sensor sensors[]){
    return SensorCount;
}

__uint8_t initSensorArray(Sensor sensors[]){
    return 0;
}
__uint8_t readTempSensors(Sensor sensors[]){
    return 0;
}
float determineTemp(Sensor sensors[]){
    return 0;
}
void setAlerts(Sensor sensors[]){}
__uint8_t logData(float displayTemp, Sensor sensors[]){
    return 0;
}
void clearFaults(Sensor sensors[]){}

#endif