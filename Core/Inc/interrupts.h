#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include <string.h>

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim16;
extern volatile uint8_t readNow;
extern uint8_t RTC_SetTime(void);
extern volatile uint8_t usartMessage[BUFFER_SIZE];  //A buffer to store the received messages via USART

extern volatile uint8_t fullMessageReceived; //True if a full message has been received via USART

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void setTIMInterrupt();

void setUSARTInterrupt();

void interruptFlagHandler();