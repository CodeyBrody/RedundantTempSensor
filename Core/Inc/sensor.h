#ifndef SENSOR_H
#define SENSOR_H
#include "hardware.h"
#include "main.h"
#include <math.h>

typedef struct
{
    uint8_t address;
    float currTemp;
    uint8_t faults;
    uint8_t lastFaults;
    struct RYGLed RYG;
} Sensor;

extern I2C_HandleTypeDef hi2c1;

void setupSensors(uint8_t SensorCount, Sensor sensors[]);
uint8_t readTempSensors(Sensor sensors[]);
void clearFaults(Sensor sensors[]);

#endif