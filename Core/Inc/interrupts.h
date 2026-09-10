#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include <string.h>

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim16;
extern volatile uint8_t readNow;
extern uint8_t RTC_SetTime(void);

/*Variables for receiving USART transmission data*/
extern uint8_t rxBuf[BUFFER_SIZE];  //Create a buffer to receive data over USART
extern uint8_t rxByte; //To hold the next incoming byte
extern uint8_t rxIndex; //To keep track of the index for the next byte in the rxBuf

extern volatile uint8_t receiveTimeData; //Used to determine if data to set time is about to be recieved
extern volatile uint8_t rtcSetError; //Used to determine if an error has occured setting the RTC and, if so, which one.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void setTIMInterrupt();

void setUSARTInterrupt();

void interruptFlagHandler();