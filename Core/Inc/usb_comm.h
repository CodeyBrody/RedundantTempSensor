#ifndef USB_COMM_H
#define USB_COMM_H
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "string.h"


extern __uint8_t txBuf[];
extern UART_HandleTypeDef huart2;


void usb_print(const char *message);
void print_temp_c(float temp);

enum messageType {
    E, // Error message type
    D, // Data message type
    S, // System data type
};

void usb_log_message(enum messageType message_type, const char *message_body){
    
    //Print the message header
    switch (message_type){
        case E:
            strcpy((char*)txBuf, "E: ");
            break;
        case D:
            strcpy((char*)txBuf, "D: ");
            break;
        case S:
            strcpy((char*)txBuf, "S: ");
            break;
    }

    //Send the message header
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

    //Send the message body
    strcpy((char*)txBuf, message_body);
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

    //Send carriage return and newline
    strcpy((char*)txBuf, "\n\r");
    HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

    return;
}

#endif /*USB_COM_H*/