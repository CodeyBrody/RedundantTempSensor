#include "stm32l4xx_hal.h"
#include "hardware.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Handle definitions to be given to the fakes*/
I2C_HandleTypeDef hi2c1;
// UART_HandleTypeDef huart2;
// SPI_HandleTypeDef hspi2;
RTC_HandleTypeDef hrtc;
// TIM_HandleTypeDef htim16;

volatile HAL_StatusTypeDef fake_HAL_Transmit_StatusReturn = HAL_OK;
volatile HAL_StatusTypeDef fake_HAL_Transmit_StatusReturn_2 = HAL_OK;
uint8_t fake_HAL_Transmit_CallCount = 0;

volatile HAL_StatusTypeDef fake_HAL_Receive_StatusReturn = HAL_OK;
volatile HAL_StatusTypeDef fake_HAL_Receive_StatusReturn_2 = HAL_OK;
uint8_t fake_HAL_Receive_CallCount = 0;

volatile float fakeTempSensorVals[SENSOR_COUNT] = {0, 0, 0};

uint8_t fakeMissingSensorAddress[SENSOR_COUNT] = {0};

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                          uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    fake_HAL_Transmit_CallCount++;
    for (uint8_t i = 0; i < SENSOR_COUNT; i++)
    {
        if (DevAddress == fakeMissingSensorAddress[i])
        {
            return HAL_ERROR;
        }
    }
    if ((fake_HAL_Transmit_CallCount % 2) == 1)
    {
        return fake_HAL_Transmit_StatusReturn;
    }
    else
    {
        return fake_HAL_Transmit_StatusReturn_2;
    }
}
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                         uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    fake_HAL_Receive_CallCount++;

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
            return HAL_ERROR;
    }

    if (temp > 127.9375 || temp < -128)
    {
        return HAL_ERROR;
    }

    int16_t encoded = (int16_t)(temp / 0.0625);
    encoded &= 0x0FFF; // Cuts off all but the least significant 12 bits

    uint8_t secondByte = (uint8_t)(encoded << 4);
    uint8_t firstByte = (uint8_t)(encoded >> 4);

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

    if (fake_HAL_Receive_CallCount % 2 == 1)
    {
        return fake_HAL_Receive_StatusReturn;
    }
    else
    {
        return fake_HAL_Receive_StatusReturn_2;
    }
}

/*RTC fakes*/

int fakeRTCSetTimeCallCount = 0;
int fakeRTCSetDateCallCount = 0;

RTC_TimeTypeDef fakeLastTime;
RTC_DateTypeDef fakeLastDate;

HAL_StatusTypeDef fakeRTCSetTimeReturn = HAL_OK;
HAL_StatusTypeDef fakeRTCSetDateReturn = HAL_OK;

HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format)
{
    fakeRTCSetTimeCallCount++;
    fakeLastTime = *sTime;
    return fakeRTCSetTimeReturn;
}

HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format)
{
    fakeRTCSetDateCallCount++;
    fakeLastDate = *sDate;
    return fakeRTCSetDateReturn;
}