#include "interrupts.h"
#include "defines.h"
#include "error_codes.h"
#include "logging.h"
#include "string.h"
#include "usb_comm.h"

uint8_t rxBuf[BUFFER_SIZE];           // Create a buffer to receive incoming data over USART
uint8_t rxByte;                       // To hold the next incoming byte
uint8_t rxIndex = 0;                  // To keep track of the index for the next byte in the rxBuf
volatile uint16_t interruptError = 0; // A variable to both signal that an error has occured in an
                                      // interrupt, and to signify which error occured

volatile uint8_t fullMessageReceived = 0; // A flag to signal when a full message has been received

void interruptErrorHandler()
{
    if (interruptError)
    {
        logError(interruptError, -1);
        interruptError = 0;
    }
    return;
}

void setTIMInterrupt()
{
    HAL_StatusTypeDef status;

    /*Set the timer interrupt to sample the sensor measurements every
     * REQUESTED_SENSOR_SAMPLING_INTERVAL_SEC seconds*/
    __HAL_TIM_SET_AUTORELOAD(&htim16, REQUESTED_SENSOR_SAMPLING_INTERVAL_SEC * 10000 - 1);
    status = HAL_TIM_Base_Start_IT(&htim16);

    if (status != HAL_OK)
    {
        Error_Handler();
    }
    return;
}

void setUSARTInterrupt()
{
    HAL_StatusTypeDef status;

    /*Set the USART interrupt*/
    status = HAL_UART_Receive_IT(&huart2, &rxByte, 1);

    if (status != HAL_OK)
    {
        Error_Handler();
    }
    return;
}

/*Function to handle the timer interrupts*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /*Check which version of the timer triggered this callback, and if TIM16 (timer for reading
      sensors),...
      ...set the system to read the temperature on the next pass through the main() while loop*/
    if (htim->Instance == TIM16)
    {
        readNow = 1;
    }

    return;
}

/*Function to handle the USART interrupts*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        // Receive the entire transmission
        if (rxByte != '\n')
        {
            if (rxIndex < BUFFER_SIZE - 1)
            {
                if (rxByte != '\r')
                {
                    rxBuf[rxIndex++] = rxByte;
                }
            }
            else
            {
                rxBuf[rxIndex] = '\0';

                // Log a buffer overflow error
                interruptError = RX_BUFFER_OVERFLOW;

                // Reset the buffer
                rxIndex = 0;
            }
        }
        else
        {
            // Full message received
            rxBuf[rxIndex] = '\0';

            if (fullMessageReceived == 0)
            { // If any prior messages have already been processed...
                strncpy((char *)usartMessage, (const char *)rxBuf,
                        BUFFER_SIZE); //...store the data from rxBuf in usartMessage...
                fullMessageReceived = 1;
                // Reset the rxBuf buffer:
                rxIndex = 0;
            }
            else // If a currently stored message has not yet been processed...
            {
                //...log an `RX_BUFFER_FULL` error, and reset the rxBuf buffer:
                interruptError = RX_BUFFER_FULL;
                rxIndex = 0;
            }
        }

        HAL_StatusTypeDef status;

        // Restart reception of data
        status = HAL_UART_Receive_IT(&huart2, &rxByte, 1);

        if (status != HAL_OK)
        {
            Error_Handler();
        }
        return;
    }
}
