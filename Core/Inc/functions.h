#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "sensor.h"

extern Sensor sensors[];

float determineTemp(Sensor sensors[]);
void setAlerts(Sensor sensors[]);
__uint8_t logData(float displayTemp, Sensor sensors[]);


float determineTemp(Sensor sensors[]){
    return 0;
}
void setAlerts(Sensor sensors[]){}

__uint8_t logData(float displayTemp, Sensor sensors[]){
    return 0;
}

#endif