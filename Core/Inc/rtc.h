#ifndef RTC_H
#define RTC_H
#include "stdint.h"

extern uint16_t current_century;

int8_t RTC_SetTime(void);

#endif /*RTC_H*/