#include "interrupts.h"
#include "error_codes.h"
#include "logging.h"
#include "usb_comm.h"

uint8_t rxBuf[BUFFER_SIZE];  //Create a buffer to receive incoming data over USART
uint8_t rxByte; //To hold the next incoming byte
uint8_t rxIndex = 0; //To keep track of the index for the next byte in the rxBuf
uint16_t interruptError = 0;

volatile uint8_t fullMessageReceived = 0;

void interruptFlagHandler(){
  if (interruptError){
    logError(interruptError, -1);
    interruptError = 0;
  }
  return;
}

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
    if (rxByte != '\n'){
      fullMessageReceived = 0;
        if(rxIndex < BUFFER_SIZE - 1){
          if(rxByte != '\r'){
            rxBuf[rxIndex++] = rxByte;
          } else {
            rxIndex++;
          }
        } else {
          rxBuf[rxIndex] = '\0';

          //Log a buffer overflow error
          interruptError = RX_BUFFER_OVERFLOW;

          //Reset the buffer
          rxIndex = 0;
        }
    }
    else{
      //Full message received
      rxBuf[rxIndex] = '\0';
      
      if(fullMessageReceived == 0){ //If any prior message has already been processed...
        strncpy((uint8_t *)usartMessage, rxBuf, BUFFER_SIZE);  //...store the data from rxBuf in usartMessage...    
        fullMessageReceived = 1;
        //Reset the rxBuf buffer:
        rxIndex = 0;
      } else {
        logError(RX_BUFFER_FULL, -1);
      }
    }

    //Restart reception of data
    HAL_UART_Receive_IT(
      &huart2,
      &rxByte,
      1
    );
  }
}
