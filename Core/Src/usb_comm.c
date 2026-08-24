#include "usb_comm.h"


extern __uint8_t txBuf[];
extern UART_HandleTypeDef huart2;

void print_temp_c(float temp){
   float d_temp = temp*100;
    sprintf((char*)txBuf,
                "%u.%02u C\r\n",
                ((unsigned int)temp / 100),
                ((unsigned int)temp % 100));
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);
}

void usb_print(const char *message_body){
    //Send the message body
    strcpy((char*)txBuf, message_body);
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

    //Send carriage return and newline
    strcpy((char*)txBuf, "\n\r");
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

    return;
}
