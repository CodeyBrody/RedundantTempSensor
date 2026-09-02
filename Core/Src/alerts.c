#include "alerts.h"
#include "hardware.h"
#include "main.h"

static __uint16_t LEDVals = 0; //Initializes a variable that is used to control the shift register outputs 

void setSensorLeds(Sensor sensors[SENSOR_COUNT]);

void setLEDValue(__uint8_t LEDNumber, __uint16_t LEDValue);

void shiftRegWrite(const __uint16_t LEDVals);

void setAlerts(Sensor sensors[]){
  setSensorLeds(sensors);
  return;
}

void setSensorLeds(Sensor sensors[SENSOR_COUNT]){
  for(int i = 0; i<SENSOR_COUNT; i++){
    __uint8_t flags = sensors[i].faults;
    if(flags){ /*If not valid...*/
      if(flags & COMM_FAULT){ /*If Failure, set LED to red...*/
        setLEDValue(sensors[i].RYG.RED, 1);
        setLEDValue(sensors[i].RYG.YELLOW, 0);
        setLEDValue(sensors[i].RYG.GREEN, 0);
      } else{ /*...else if invalid, set the yellow LED...*/
        setLEDValue(sensors[i].RYG.RED, 0);
        setLEDValue(sensors[i].RYG.YELLOW, 1);
        setLEDValue(sensors[i].RYG.GREEN, 0);
      } /*...and if it is valid, set LED to green.*/
    } else {
        setLEDValue(sensors[i].RYG.RED, 0);
        setLEDValue(sensors[i].RYG.YELLOW, 0);
        setLEDValue(sensors[i].RYG.GREEN, 1);
    }
  }
}

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

void shiftRegWrite(const __uint16_t LEDVals){
  HAL_GPIO_WritePin(LATCH_GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, (__uint8_t*)&LEDVals, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(LATCH_GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);
}