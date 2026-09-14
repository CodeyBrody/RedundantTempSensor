#include "rtc.h"
#include "usb_comm.h"
#include "defines.h"
#include "interrupts.h"
#include "ctype.h"

extern uint8_t rxBuf[BUFFER_SIZE];  //A buffer to receive data over USART
extern RTC_HandleTypeDef hrtc;
extern volatile uint8_t receiveTimeData;


uint8_t calculate_time(uint8_t firstDigit, uint8_t secondDigit){
  return (
    (rxBuf[firstDigit] - '0')*10 + 
    (rxBuf[secondDigit] - '0')
  );
}

uint8_t check_bounds(uint8_t num, uint8_t upper_bound, uint8_t lower_bound){
  return (num <= upper_bound && num >= lower_bound);
}

uint8_t SakamotoAlgo(RTC_DateTypeDef sDate){
  uint8_t t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

  if (sDate.Month < 3) {sDate.Year -= 1;}

  return ((sDate.Year + sDate.Year/4 - sDate.Year/100 + sDate.Year/400 + t[sDate.Month-1]+sDate.Date) % 7);
}

uint8_t RTC_checkTimeInput(const char *input, RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime){

  /*Check that the received string is the correct size*/
  if(strlen((const char*)input) != 19){
    return 1;
  }
  
  /*Check to make sure all the numbers intended to be digits are digits*/
  for(int i = 2; i < 19; i++){
    if((i % 3) != 1){
      if(!isdigit(input[i])){
        return 1;
      }
    }
  }

  //Check the non-digit characters of the expected formatted time string
  if(input[4] != '/' || input[7] != '/' || input[10] != ' ' || input[13] != ':' || input[16] != ':'){
    return 1;
  }

  uint8_t calculated_entry = 0;

  sDate->Year = calculate_time(2, 3);

  calculated_entry = calculate_time(5, 6);
  
  if(!(calculated_entry <= 12 && calculated_entry >= 1)){
    return 1;
  } else {
    sDate->Month = calculated_entry;
  }

    calculated_entry = calculate_time(8, 9);
  
    /*Ehnance this...*/
  if(!(calculated_entry <= 31 && calculated_entry >= 1)){
    return 1;
  } else {
    sDate->Date = calculated_entry;
  }

  sDate->WeekDay = SakamotoAlgo(*sDate);

  calculated_entry = calculate_time(11, 12);

  if(!(calculated_entry <= 23 && calculated_entry >= 0)){
    return 1;
  } else {
    sTime->Hours = calculated_entry;
  }

  calculated_entry = calculate_time(14, 15);

  if(!(calculated_entry <= 59 && calculated_entry >= 0)){
    return 1;
  } else {
    sTime->Minutes = calculated_entry;
  }

  calculated_entry = calculate_time(17, 18);

  if(!(calculated_entry <= 59 && calculated_entry >= 0)){
    return 1;
  } else {
    sTime->Seconds = calculated_entry;
  }

  return 0;

}

/*A function to set the RTC time. Returns 0 on success, 1 for formatted string errors, 2 for errors attempting HAL_SET time or date*/
uint8_t RTC_SetTime(void){

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  if(RTC_checkTimeInput((const char *)rxBuf, &sDate, &sTime)){
    //If RTC_checkTimeInput returned 1, then a formatting error was encountered...pass it along by also returning 1
    return 1;
  } 

  //If the program proceeds here, should be formatted correctly. Now to set the time and check the return type.
  HAL_StatusTypeDef ret = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  if(ret != HAL_OK){
    return 2;
  }
  ret = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  if(ret != HAL_OK){
    return 2;
  }

  return 0;
}

void RTC_RequestTime(void){
    receiveTimeData = 1;
    usb_println("SEND_TIME");
}