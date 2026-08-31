#ifndef ALERTS_H
#define ALERTS_H
#include "sensor.h"
#include "faults.h"
#include "main.h"
#include "hardware.h"

extern SPI_HandleTypeDef hspi1;

static __uint16_t LEDVals = 0; //Initializes a variable that is used to control the shift register outputs 

void setAlerts(Sensor sensors[]);

#endif