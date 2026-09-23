#ifndef ALERTS_H
#define ALERTS_H
#include "faults.h"
#include "hardware.h"
#include "main.h"
#include "sensor.h"


extern SPI_HandleTypeDef hspi1;

void setAlerts(Sensor sensors[]);

void updateBuzzer(void);

#endif