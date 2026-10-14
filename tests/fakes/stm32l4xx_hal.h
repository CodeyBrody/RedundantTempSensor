#ifndef FAKE_STM32L$XX_HAL_H
#define FAKE_STM32L$XX_HAL_H
#include "stdint.h"

typedef enum
{
    HAL_OK = 0x00U,
    HAL_ERROR = 0x01U,
    HAL_BUSY = 0x02U,
    HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

/** ==============================
 * Dummy Peripheral Register Types
 */

typedef struct
{
    uint32_t dummy;
} USART_TypeDef;
typedef struct
{
    uint32_t dummy;
} TIM_TypeDef;
typedef struct
{
    uint32_t dummy;
} GPIO_TypeDef;
typedef struct
{
    uint32_t dummy;
} RTC_TypeDef;
typedef struct
{
    uint32_t dummy;
} I2C_TypeDef;
typedef struct
{
    uint32_t dummy;
} SPI_TypeDef;

/**
 * Fake Hardware Instances
 */

extern USART_TypeDef fake_USART2;
extern TIM_TypeDef fake_TIM16;

extern GPIO_TypeDef fake_GPIOA;
extern GPIO_TypeDef fake_GPIOB;
extern GPIO_TypeDef fake_GPIOC;

#define USART2 (&fake_USART2)
#define TIM16 (&fake_TIM16)

#define GPIOA (&fake_GPIOA)
#define GPIOB (&fake_GPIOB)
#define GPIOC (&fake_GPIOC)

/**
 * GPIO
 */

typedef enum
{
    GPIO_PIN_RESET = 0U,
    GPUO_PIN_SET
} GPIO_PinState;

#define GPIO_PIN_2 ((uint16_t)0x0004)
#define GPIO_PIN_3 ((uint16_t)0x0008)
#define GPIO_PIN_4 ((uint16_t)0x0010)
#define GPIO_PIN_7 ((uint16_t)0x0080)
#define GPIO_PIN_13 ((uint16_t)0x2000)
#define GPIO_PIN_14 ((uint16_t)0x4000)

/**
 * HAL handles
 */

typedef struct
{
    I2C_TypeDef *Instance;
} I2C_HandleTypeDef;

typedef struct
{
    USART_TypeDef *Instance;
} UART_HandleTypeDef;

typedef struct
{
    RTC_TypeDef *Instance;
} RTC_HandleTypeDef;

typedef struct
{
    uint32_t Period;
} TIM_Base_InitTypeDef;

typedef struct
{
    TIM_TypeDef *Instance;
    TIM_Base_InitTypeDef Init;
} TIM_HandleTypeDef;

typedef struct
{
    SPI_TypeDef *Instance;
} SPI_HandleTypeDef;

/**
 * RTC
 */

typedef struct
{
    uint8_t Hours;
    uint8_t Minutes;
    uint8_t Seconds;
    uint32_t TimeFormat;
    uint32_t SubSeconds;
    uint32_t SecondsFraction;
    uint32_t DayLightSaving;
    uint32_t StoreOperation;
} RTC_TimeTypeDef;

typedef struct
{
    uint8_t WeekDay;
    uint8_t Month;
    uint8_t Date;
    uint8_t Year;
} RTC_DateTypeDef;

#define RTC_FORMAT_BIN 0x00000000U

/**
 * Function Declarations
 */

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                          uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
                                         uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);

HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);

HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);

HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

uint32_t HAL_GetTick(void);

HAL_StatusTypeDef HAL_Init(void);

/**
 * CMSIS / HAL macros
 */

void fake_disable_irq(void);

#define __disable_irq() fake_disable_irq()

#define __HAL_TIM_SET_AUTORELOAD(__HANDLE__, __VALUE__) \ ((__HANDLE__)->Init.Period = (__VALUE__))

#endif