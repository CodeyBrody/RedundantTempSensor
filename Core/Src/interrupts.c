#include "interrupts.h"

__uint8_t rxBuf[BUFFER_SIZE];  //Create a buffer to receive data over USART
__uint8_t rxByte; //To hold the next incoming byte
__uint8_t rxIndex = 0; //To keep track of the index for the next byte in the rxBuf
volatile uint8_t setTime = 0; //Used to determine if data to set time is about to be recieved

void setTIMInterrupt(){
    /*Set the timer interrupt to sample the sensor measurements every DELTA seconds*/
  __HAL_TIM_SET_AUTORELOAD(&htim16, DELTA * 10000 - 1);
    HAL_TIM_Base_Start_IT(&htim16);
    return;
}

void setUSARTInterrupt(){
    
    HAL_StatusTypeDef status;
    
    /*Set the USART interrupt*/
    status = HAL_UART_Receive_IT(
        &huart2,
        &rxByte,
        1
    );

    if(status != HAL_OK){
        Error_Handler();
    }
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
    if (rxByte != '\n' && rxByte != '\r'){
        if(rxIndex < BUFFER_SIZE - 1){
          rxBuf[rxIndex++] = rxByte;
        }
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
