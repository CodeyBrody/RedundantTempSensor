#ifndef USB_COMM_H
#define USB_COMM_H
#include "main.h"

extern UART_HandleTypeDef huart2;

void usb_print(const char *message);

void usb_printf_int(const char *message, int arg);

void print_temp(float temp);

void usb_print_delimiter(const char *delimiter);

void usb_println(const char *message_body);

#endif /*USB_COM_H*/