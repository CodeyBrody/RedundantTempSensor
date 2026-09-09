#ifndef STM32_H
#define STM32_H
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

extern RTC_HandleTypeDef hrtc;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim16;

extern UART_HandleTypeDef huart2;

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_I2C1_Init(void);
void MX_USART2_UART_Init(void);
void MX_RTC_Init(void);
void MX_SPI1_Init(void);
void MX_TIM16_Init(void);

#endif /*STM32_H*/