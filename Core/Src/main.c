/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "defines.h"
#include "sensor.h"
#include "hardware.h"
#include "interrupts.h"
#include "alerts.h"
#include "determination_logic.h"
#include "logging.h"
#include "math.h"
#include "error_codes.h"
#include "rtc.h"
#include "stm32.h"
#include "usb_comm.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static Sensor sensors[SENSOR_COUNT];
float displayTemp = NAN;

//A variable to determine whether enough time has passed since the last read session
volatile uint8_t readNow = 1;

/*USART Message Handling Variables*/
uint8_t setTime = 0; //Used to tell the system that the RTC is ready to be set
uint8_t sendTimeMessage = 0; //Used to determine whether or not to send 'SEND_TIME' message via USART
volatile uint8_t usartMessage[BUFFER_SIZE]; //A buffer to hold the complete received USART messages

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
void prepNextSensorRead(void);
void usartMessageHandler(void);
void RTC_RequestTime(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  /*Discover currently communicating temp sensors and initialize the structs for the sensors*/
  setupSensors(SENSOR_COUNT, sensors);

  //Setup timer 16 interrupt for reading the temp sensor registers every delta...
  setTIMInterrupt();
  
  //Setup the USART Interrupt...
  setUSARTInterrupt();

  //Request Info to set RTC Clock
  RTC_RequestTime();

  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(readNow){
      readNow = 0;
      if(!readTempSensors(sensors)){
        logError(ALL_SENSOR_READS_MISSING, -1);
      }
      displayTemp = determineTemp(sensors);
      logData(displayTemp, sensors);
      setAlerts(sensors);
      prepNextSensorRead();
    }
    usartMessageHandler();
    interruptFlagHandler();
    updateBuzzer(); // Turns off the buzzer if the buzzer is active and no longer supposed to be on
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

void prepNextSensorRead(void){
  clearFaults(sensors);
  displayTemp = NAN;
}

void RTC_RequestTime(void){
    setTime = 1;
    usb_println("SEND_TIME");
}

void usartMessageHandler(void){
      //Process the received transmission information
      if(fullMessageReceived){
        if(setTime == 1){
          //If the program progresses here, it should have received the time data
          //reset the 'receiveTimeData' variable and set 'setTime', to attempt to set the RTC with the received data
              uint8_t returnCode = RTC_SetTime();
              if(returnCode){ //If the time is not accurately set...
                switch (returnCode){
                  case 1: logError(RTC_FORMATTING_ERROR, -1); //...log an error
                          break;
                  case 2: logError(RTC_SET_ERROR, -1); //...or maybe this error
                          break;
                  default:
                          break;
                }
                usb_print("SEND_TIME\r\n");
                setTime = 1;
              } else {
                setTime = 0;
              }
        } else if(strcmp((const char*)usartMessage, "SET_TIME") == 0){ //If "SET_TIME" was received...
          //Set the boolean variable to set the time on the next pass through
          setTime = 1;
          usb_print("SEND_TIME\r\n");
        } else {
          logError(UNRECOGNIZED_COMMAND_RECEIVED, -1);
        }
        /*CAN ADD FUTURE USART COMMANDS TO BE PROCESSED TO THIS CHAINED IF-ELSE BLOCK*/
        fullMessageReceived = 0;
      }      
    return;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
