#include "usb_comm.h"
#include "defines.h"
#include "math.h"

uint8_t txBuf[BUFFER_SIZE];
extern UART_HandleTypeDef huart2;

void print_temp_c(float temp){
    if(isnan(temp)){
        snprintf((char*)txBuf, BUFFER_SIZE, "--.--");
    }
    else {
        float d_temp = temp*100;
        snprintf((char*)txBuf, BUFFER_SIZE,
                    "%u.%02u",
                    ((signed int)d_temp / 100),
                    ((signed int)d_temp % 100));
    }
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), USART_TX_TIMEOUT_MS);
}

void usb_print(const char *message_body){
    //Send the message body
    HAL_UART_Transmit(&huart2, (uint8_t *)message_body, strlen(message_body), USART_TX_TIMEOUT_MS);
    return;
}

void usb_print_delimiter(const char *delimiter){
    HAL_UART_Transmit(&huart2, (const uint8_t *)delimiter, strlen(delimiter), USART_TX_TIMEOUT_MS);
}

void usb_println(const char *message_body){
    //Send the message body
    HAL_UART_Transmit(&huart2, (const uint8_t *)message_body, strlen(message_body), USART_TX_TIMEOUT_MS);

    //Send carriage return and newline
    HAL_UART_Transmit(&huart2, (const uint8_t *)"\r\n", 2, USART_TX_TIMEOUT_MS);

    return;
}

void usb_printf_int(const char *message, int arg){
    snprintf((char*)txBuf, BUFFER_SIZE, message, arg);
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), USART_TX_TIMEOUT_MS);
}