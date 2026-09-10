#include "interrupts.h"
#include "error_codes.h"
#include "logging.h"

uint8_t rxBuf[BUFFER_SIZE];  //Create a buffer to receive data over USART
uint8_t rxByte; //To hold the next incoming byte
uint8_t rxIndex = 0; //To keep track of the index for the next byte in the rxBuf
uint16_t interruptError = 0;

volatile uint8_t receiveTimeData = 0; //Used to determine if data to set time is about to be recieved
uint8_t setTime = 0; //Used to tell the system that the RTC is ready to be set
uint8_t sendTimeMessage = 0; //Used to determine whether or not to send 'SEND_TIME' message via USART

void interruptFlagHandler(){
  if (receiveTimeData && sendTimeMessage){
    snprintf((char*)rxBuf, BUFFER_SIZE, "SEND_TIME\r\n");
    HAL_UART_Transmit(&huart2, rxBuf, strlen((char*)rxBuf), HAL_MAX_DELAY);
    sendTimeMessage = 0;
  }

  if(setTime){
    if(!RTC_SetTime()){ //If the time is not accurately set...
      interruptError = RTC_FORMATTING_ERROR; //flag an error
      setTime = 0;
      sendTimeMessage = 1;
      receiveTimeData = 1;
    }
  }

  if (interruptError){
    logError(interruptError, -1);
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
    if (rxByte != '\n' && rxByte != '\r'){
        if(rxIndex < BUFFER_SIZE - 1){
          rxBuf[rxIndex++] = rxByte;
        }
    }
    else{
      //Full message received
      rxBuf[rxIndex] = '\0';

      //Process the received transmission information

      if(receiveTimeData == 1){
        //If the program progresses here, it should have received the time data
        //reset the 'receiveTimeData' variable and set 'setTime', to attempt to set the RTC with the received data
        receiveTimeData = 0;
        setTime = 1;
      }

      /*CAN ADD FUTURE USART COMMANDS TO BE PROCESSED TO THIS CHAINED IF-ELSE BLOCK*/
      //If "SET_TIME" was received...
      if(strcmp((const char*)rxBuf, "SET_TIME") == 0){
        //Set the boolean variable to set the time on the next pass through
        receiveTimeData = 1;
        sendTimeMessage = 1;
      } else {
        interruptError = UNRECOGNIZED_COMMAND_RECEIVED;
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
