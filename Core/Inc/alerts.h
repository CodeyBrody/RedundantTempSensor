#ifndef ALERTS_H
#define ALERTS_H
#include "sensor.h"
#include "faults.h"
#include "main.h"
#include "hardware.h"

extern SPI_HandleTypeDef hspi1;

static __uint16_t LEDVals = 0; //Initializes a variable that is used to control the shift register outputs 

void setAlerts(Sensor sensors[]);

void setAlerts(Sensor sensors[]){}

void setLEDValue(__uint8_t LEDNumber, __uint16_t LEDValue){
  if(LEDValue){
    LEDVals |= (1<<LEDNumber);
  } 
  else {
    LEDVals &= ~(1<<LEDNumber);
  }
  shiftRegWrite(LEDVals);

  return;
}

void shiftRegWrite(const __uint16_t LEDVals);
void setSensorLeds(Sensor sensors[SENSOR_COUNT]);

#endif