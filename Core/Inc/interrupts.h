#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include <string.h>

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim16;
extern volatile __uint8_t readNow;
extern void RTC_SetTime(void);

/*Variables for receiving USART transmission data*/
extern __uint8_t rxBuf[BUFFER_SIZE];  //Create a buffer to receive data over USART
extern __uint8_t rxByte; //To hold the next incoming byte
extern __uint8_t rxIndex; //To keep track of the index for the next byte in the rxBuf
extern volatile uint8_t setTime; //Used to determine if data to set time is about to be recieved

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void setTIMInterrupt();

void setUSARTInterrupt();