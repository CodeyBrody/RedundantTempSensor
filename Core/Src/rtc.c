#include "rtc.h"
#include "main.h"
#include "usb_comm.h"
#include "defines.h"
#include "interrupts.h"

extern __uint8_t rxBuf[BUFFER_SIZE];  //A buffer to receive data over USART
extern RTC_HandleTypeDef hrtc;
extern volatile uint8_t setTime;


__uint8_t SakamotoAlgo(RTC_DateTypeDef sDate){
  __uint8_t t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

  if (sDate.Month < 3) {sDate.Year -= 1;}

  return ((sDate.Year + sDate.Year/4 - sDate.Year/100 + sDate.Year/400 + t[sDate.Month-1]+sDate.Date) % 7);
}

void RTC_SetTime(void){

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  sDate.WeekDay = SakamotoAlgo(sDate);
 
  sDate.Year = (
    (rxBuf[2] - '0')*10 + 
    (rxBuf[3] - '0')
  );

  sDate.Month = (
    (rxBuf[5] - '0')*10 + 
    (rxBuf[6] - '0')
  );

  sDate.Date = (
    (rxBuf[8] - '0')*10 + 
    (rxBuf[9] - '0')
  );

    sTime.Hours = (
    (rxBuf[11] - '0')*10 + 
    (rxBuf[12] - '0')
  );

  sTime.Minutes = (
    (rxBuf[14] - '0')*10 + 
    (rxBuf[15] - '0')
  );

  sTime.Seconds = (
    (rxBuf[17] - '0')*10 + 
    (rxBuf[18] - '0')
  );

  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

}

void RTC_RequestTime(void){
    setTime = 1;
    usb_println("SEND_TIME");
}