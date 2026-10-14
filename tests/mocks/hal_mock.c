#include "stm32l4xx_hal.h"
#include <string.h>

/* Fake hardware objects */
USART_TypeDef fake_USART2;
TIM_TypeDef fake_TIM16;

GPIO_TypeDef fake_GPIOA;
GPIO_TypeDef fake_GPIOB;
GPIO_TypeDef fake_GPIOC;

/* ==============================
 * GPIO fake
 * ============================== */

int fake_gpio_write_call_count;

GPIO_TypeDef *fake_gpio_last_port;
uint16_t fake_gpio_last_pin;
GPIO_PinState fake_gpio_last_state;

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    fake_gpio_write_call_count++;

    fake_gpio_last_port = GPIOx;
    fake_gpio_last_pin = GPIO_Pin;
    fake_gpio_last_state = PinState;
}