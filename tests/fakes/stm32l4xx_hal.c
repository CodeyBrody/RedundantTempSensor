#include "stm32l4xx_hal.h"
#include "hardware.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Handle definitions to be given to the fakes*/
I2C_HandleTypeDef hi2c1;
// UART_HandleTypeDef huart2;
// SPI_HandleTypeDef hspi2;
// RTC_HandleTypeDef hrtc;
// TIM_HandleTypeDef htim16;

volatile HAL_StatusTypeDef fake_HAL_Transmit_StatusReturn = HAL_OK;
volatile HAL_StatusTypeDef fake_HAL_Transmit_StatusReturn_2 = HAL_OK;
uint8_t fake_HAL_Transmit_StatusCallCount = 0;

volatile HAL_StatusTypeDef fake_HAL_Receive_StatusReturn = HAL_OK;
volatile HAL_StatusTypeDef fake_HAL_Receive_StatusReturn_2 = HAL_OK;
uint8_t fake_HAL_Receive_StatusCallCount = 0;

volatile float fakeTempSensorVals[SENSOR_COUNT] = {0, 0, 0};

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                          uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    fake_HAL_Transmit_StatusCallCount++;
    return fake_HAL_Transmit_StatusReturn;
}
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                         uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    fake_HAL_Receive_StatusCallCount++;

    float temp;
    switch (DevAddress)
    {
        case (0x48 << 1):
            temp = fakeTempSensorVals[0];
            break;
        case (0x49 << 1):
            temp = fakeTempSensorVals[1];
            break;
        case (0x4A << 1):
            temp = fakeTempSensorVals[2];
            break;
        default:
            printf("FIrstHalERr?");
            return HAL_ERROR;
    }

    if (abs(temp) > 127)
    {
        return HAL_ERROR;
    }

    uint8_t sign = (temp < 0) << 7; // 0 = positive, 1 = negative
    int16_t encoded = abs(temp) / 0.0625;

    printf("Encoded: %d", encoded);

    int8_t secondByte = (encoded % 16) << 4;
    int8_t firstByte = (encoded >> 4);
    if (sign > 0)
    {
        firstByte |= sign;
    }
    else
    {
        firstByte &= sign;
    }

    int count = 0;
    while (count < Size)
    {
        if (count == 0)
        {
            pData[count] = firstByte;
        }
        else if (count == 1)
        {
            pData[count] = secondByte;
        }
        else
        {
            return HAL_ERROR;
        }
        count++;
    }

    return fake_HAL_Receive_StatusReturn;
}
