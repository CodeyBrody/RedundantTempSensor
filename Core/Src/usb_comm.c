#include "usb_comm.h"
#include "defines.h"
#include "math.h"

extern __uint8_t txBuf[];
extern UART_HandleTypeDef huart2;

void print_temp_c(float temp){
    if(isnan(temp)){
        sprintf((char*)txBuf, "--.-- C");
    }
    else {
        float d_temp = temp*100;
        sprintf((char*)txBuf,
                    "%u.%02u C",
                    ((unsigned int)d_temp / 100),
                    ((unsigned int)d_temp % 100));
    }
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);
}

void usb_print(const char *message_body){
    //Send the message body
    strcpy((char*)txBuf, message_body);
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);
    return;
}

void usb_print_delimiter(const char *delimiter){
    strcpy((char*)txBuf, delimiter);
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);
}

void usb_println(const char *message_body){
    //Send the message body
    strcpy((char*)txBuf, message_body);
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

    //Send carriage return and newline
    strcpy((char*)txBuf, "\n\r");
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

    return;
}

void usb_printf_int(const char *message, int arg){
    snprintf((char*)txBuf, BUFFER_SIZE, message, arg);
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);
}