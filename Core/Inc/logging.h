#ifndef LOGGING_H
#define LOGGING_H
#include "main.h"
#include "sensor.h"

extern __uint8_t txBuf[];
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;


__uint8_t logData(float displayTemp, Sensor sensors[]);
void logError(int Error, int optionalInt);
void logCurrentDateTime(__uint8_t txBuf[]);

// enum messageType {
//     E, // Error message type
//     D, // Data message type
//     S, // System data type
// };

// void usb_log_message(enum messageType message_type, const char *message_body){
    
    // // Print the message header
//     switch (message_type){
//         case E:
//             strcpy((char*)txBuf, "E: ");
//             break;
//         case D:
//             strcpy((char*)txBuf, "D: ");
//             break;
//         case S:
//             strcpy((char*)txBuf, "S: ");
//             break;
//     }

//     //Send the message header
//     HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

//     //Send the message body
//     strcpy((char*)txBuf, message_body);
//     HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

//     //Send carriage return and newline
//     strcpy((char*)txBuf, "\n\r");
//     HAL_UART_Transmit(&huart2, txBuf, strlen((char*)txBuf), HAL_MAX_DELAY);

//     return;
// }

#endif LOGGING_H