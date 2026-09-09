#include "rtc.h"
#include "usb_comm.h"
#include "defines.h"
#include "interrupts.h"
#include "ctype.h"

extern __uint8_t rxBuf[BUFFER_SIZE];  //A buffer to receive data over USART
extern RTC_HandleTypeDef hrtc;
extern volatile uint8_t setTime;

uint8_t calculate_time(uint8_t firstDigit, uint8_t secondDigit){
  return (
    (rxBuf[firstDigit] - '0')*10 + 
    (rxBuf[secondDigit] - '0')
  );
}

uint8_t check_bounds(uint8_t num, uint8_t upper_bound, uint8_t lower_bound){
  return (num <= upper_bound && num >= lower_bound);
}

__uint8_t SakamotoAlgo(RTC_DateTypeDef sDate){
  __uint8_t t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

  if (sDate.Month < 3) {sDate.Year -= 1;}

  return ((sDate.Year + sDate.Year/4 - sDate.Year/100 + sDate.Year/400 + t[sDate.Month-1]+sDate.Date) % 7);
}

uint8_t RTC_SetTime(void){

  /*Check that the received string is the correct size*/
  if(strlen(rxBuf) != 18){
    return 0;
  }
  
  /*Check to make sure all the numbers intended to be digits are digits*/
  for(int i = 2; i < 18; i++){
    if((i % 3) == 1){
      if(!isdigit(rxBuf[i])){
        return 0;
      }
    }
  }

  uint8_t calculated_entry = 0;

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  
  sDate.Year = calculate_time(2, 3);

  calculated_entry = calculate_time(5, 6);
  
  if(!(calculated_entry <= 12 && calculated_entry >= 1)){
    return 0;
  } else {
    sDate.Month = calculated_entry;
  }

    calculated_entry = calculate_time(8, 9);
  
  if(!(calculated_entry <= 31 && calculated_entry >= 1)){
    return 0;
  } else {
    sDate.Date = calculated_entry;
  }

  sDate.WeekDay = SakamotoAlgo(sDate);

  calculated_entry = calculate_time(11, 12);

  if(!(calculated_entry <= 23 && calculated_entry >= 0)){
    return 0;
  } else {
    sTime.Hours = calculated_entry;
  }

  calculated_entry = calculate_time(14, 15);

  if(!(calculated_entry <= 59 && calculated_entry >= 0)){
    return 0;
  } else {
    sTime.Minutes = calculated_entry;
  }

  calculated_entry = calculate_time(17, 18);

  if(!(calculated_entry <= 59 && calculated_entry >= 0)){
    return 0;
  } else {
    sTime.Seconds = calculated_entry;
  }

  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  setTime = 0;
  
  return 1;
}

void RTC_RequestTime(void){
    setTime = 1;
    usb_println("SEND_TIME");
}