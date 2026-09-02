#ifndef SENSOR_H
#define SENSOR_H
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "hardware.h"

typedef struct {
    __uint8_t address;
    float currTemp;
    // char *currTimeStamp; /*Can possibly use these later...for now, there's no use for them.*/
    // float lastTemp;
    // char *lastTimeStamp;
    __uint8_t faults;
    struct RYGLed RYG;
} Sensor;

extern uint8_t txBuf[];
extern I2C_HandleTypeDef hi2c1;

void setupSensors(__uint8_t SensorCount, Sensor sensors[]);
__uint8_t readTempSensors(Sensor sensors[]);
void clearFaults(Sensor sensors[]);

#endif