#ifndef ALERTS_H
#define ALERTS_H
#include "sensor.h"
#include "faults.h"
#include "main.h"
#include "hardware.h"

extern SPI_HandleTypeDef hspi1;

void setAlerts(Sensor sensors[]);

#endif