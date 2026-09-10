#ifndef LOGGING_H
#define LOGGING_H
#include "main.h"
#include "sensor.h"

extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;


void logData(float displayTemp, Sensor sensors[]);
void logError(int Error, int optionalInt);
void logCurrentDateTime(void);

#endif /*LOGGING_H*/