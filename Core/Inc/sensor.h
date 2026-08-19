#ifndef SENSOR_H
#define SENSOR_H
#include <stdlib.h>
#include <math.h>

typedef struct {
    __uint8_t address;
    float currTemp;
    char* currTimeStamp[20];
    float lastTemp;
    char* lastTimeStamp[20];
    __uint8_t faults;
} Sensor;

#endif