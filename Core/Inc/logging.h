#ifndef LOGGING_H
#define LOGGING_H
#include "main.h"
#include "sensor.h"
#include "defines.h"

extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;

extern volatile uint8_t usartMessage[BUFFER_SIZE]; //A buffer to hold the complete received USART messages in

void logData(float displayTemp, Sensor sensors[]);
void logError(int Error, int optionalInt);
void logCurrentDateTime(void);

#endif /*LOGGING_H*/