#ifndef SENSOR_H
#define SENSOR_H
#include <stdlib.h>
#include <math.h>

typedef struct {
    __uint8_t address;
    float currTemp;
    // char *currTimeStamp; /*Can possibly use these later...for now, there's no use for them.*/
    // float lastTemp;
    // char *lastTimeStamp;
    __uint8_t faults;
} Sensor;

extern SENSOR_COUNT;
extern Sensor sensors[SENSOR_COUNT];
extern uint8_t txBuf[];
extern I2C_HandleTypeDef hi2c1;

__uint8_t discoverSensorArray(__uint8_t SensorCount, Sensor sensors[]);
void initSensorArray(__uint8_t SensorCount, Sensor sensors[]);
__uint8_t readTempSensors(Sensor sensors[]);
void clearFaults(Sensor sensors[]);

#endif