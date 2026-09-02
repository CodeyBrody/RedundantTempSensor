#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include "defines.h"
#include <string.h>

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim16;
extern RTC_HandleTypeDef hrtc;
extern volatile __uint8_t readNow;

/*Variables for receiving USART transmission data*/
static __uint8_t rxBuf[BUFFER_SIZE];  //Create a buffer to receive data over USART
static __uint8_t rxByte; //To hold the next incoming byte
static __uint8_t rxIndex = 0; //To keep track of the index for the next byte in the rxBuf
static __uint8_t setTime = 0; //Used to determine if data to set time is about to be recieved

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

__uint8_t SakamotoAlgo(RTC_DateTypeDef sDate){
  __uint8_t t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

  if (sDate.Month < 3) {sDate.Year -= 1;}

  return ((sDate.Year + sDate.Year/4 - sDate.Year/100 + sDate.Year/400 + t[sDate.Month-1]+sDate.Date) % 7);
}

void RTC_SetTime(void){

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  sDate.WeekDay = SakamotoAlgo(sDate);
 
  sDate.Year = (
    (rxBuf[2] - '0')*10 + 
    (rxBuf[3] - '0')
  );

  sDate.Month = (
    (rxBuf[5] - '0')*10 + 
    (rxBuf[6] - '0')
  );

  sDate.Date = (
    (rxBuf[8] - '0')*10 + 
    (rxBuf[9] - '0')
  );

    sTime.Hours = (
    (rxBuf[11] - '0')*10 + 
    (rxBuf[12] - '0')
  );

  sTime.Minutes = (
    (rxBuf[14] - '0')*10 + 
    (rxBuf[15] - '0')
  );

  sTime.Seconds = (
    (rxBuf[17] - '0')*10 + 
    (rxBuf[18] - '0')
  );

  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

}

void setTIMInterrupt(){
    /*Set the timer interrupt to sample the sensor measurements every DELTA seconds*/
  __HAL_TIM_SET_AUTORELOAD(&htim16, DELTA * 10000 - 1);
    HAL_TIM_Base_Start_IT(&htim16);
    return;
}

void setUSARTInterrupt(){
    /*Set the USART interrupt*/
    HAL_UART_Receive_IT(
        &huart2,
        &rxByte,
        1
    );
    return;
}

/*Function to handle the timer interrupts*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  /*Check which version of the timer triggered this callback, and if TIM16 (timer for reading sensors),...
    ...set the system to read the temperature on the next pass through the main() while loop*/
  if(htim->Instance == TIM16){
    readNow = 1;
  }
}

/*Function to handle the USART interrupts*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if(huart->Instance == USART2){

    //Receive the entire transmission
    if (rxByte != '\n'){
      rxBuf[rxIndex++] = rxByte;
      HAL_UART_Transmit(&huart2, rxBuf, strlen((char*)rxBuf), HAL_MAX_DELAY);
    }
    else{
      //Full message received
      rxBuf[rxIndex] = '\0';

      //Process the received transmission information

      if(setTime == 1){
        //If the program progresses here, it should have received the time data
        RTC_SetTime();
        setTime = 0;
      }

      //If "SET_TIME" was received...
      if(strcmp((const char*)rxBuf, "SET_TIME") == 0){
        //Set the boolean variable to set the time on the next pass through
        setTime = 1;
        sprintf((char*)rxBuf, "SEND_TIME\r\n");
        HAL_UART_Transmit(&huart2, rxBuf, strlen((char*)rxBuf), HAL_MAX_DELAY);
      }
      
      rxIndex = 0;
    }

    //Restart reception of data
    HAL_UART_Receive_IT(
      &huart2,
      &rxByte,
      1
    );
  }
}
