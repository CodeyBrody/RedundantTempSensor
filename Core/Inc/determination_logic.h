#ifndef DETERMINATION_LOGIC_H
#define DETERMINATION_LOGIC_H
#include "sensor.h"

extern Sensor sensors[SENSOR_COUNT];

float determineTemp(float display_temperature, Sensor sensors[]);


#endif /*DETERMINATION_LOGIC_H*/