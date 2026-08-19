#ifndef SENSOR_H
#define SENSOR_H
#include <cstdlib>
#include <math.h>

typedef struct {
    __uint8_t address;
    float currTemp = NAN;
    char* currTimeStamp[20];
    float lastTemp = NAN;
    char* lastTimeStamp[20];
    __uint8_t faults = 0;
} Sensor;

#endif