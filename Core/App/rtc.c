#include "rtc.h"
#include "usb_comm.h"
#include "defines.h"
#include "interrupts.h"
#include "ctype.h"
#include "string.h"

extern volatile uint8_t usartMessage[BUFFER_SIZE];  //A buffer to receive data over USART
extern RTC_HandleTypeDef hrtc;


uint8_t calculate_time(uint8_t firstDigit, uint8_t secondDigit){
  return (
    (usartMessage[firstDigit] - '0')*10 + 
    (usartMessage[secondDigit] - '0')
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

/*A return value of 1 means a formatting error, a return value of 2 means an invalid date/time error. Return value of 0 indicates A-OK!*/
uint8_t RTC_checkTimeInput(const char *input, RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime){

  /*Check that the received string is the correct size*/
  if(strlen((const char*)input) != 19){
    return 1;
  }
  
  /*Check to make sure all the numbers intended to be digits are digits*/
  if(!isdigit(input[1])){
        return 1;
  }
  for(int i = 0; i < 19; i++){
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

  uint8_t year = calculate_time(2, 3);

  calculated_entry = calculate_time(5, 6);
  
  if(!(calculated_entry <= 12 && calculated_entry >= 1)){
    return 2;
  } else {
    sDate->Month = calculated_entry;
  }

  uint8_t maxDays = 0;

  switch(calculated_entry){
    case 1:  //January
    case 3:  //March
    case 5:  //May
    case 7:  //July
    case 8:  //August
    case 10: //October
    case 12: //December
      maxDays = 31;
      break;
    case 4:  //April
    case 6:  //June
    case 9:  //September
    case 11: //November
      maxDays = 30;
      break;
    case 2: //February
    //If it is a leap year
      if(year % 4 == 0){
        if((year != 0) || ((calculate_time(0,1) % 4) != 0 && year == 0)){
          maxDays = 29;
          break;
        } 
      }
      //Otherwise...
      maxDays = 28;
      break;
    default:
      //Log an INVALID_DATETIME error.
      return 2;
  }

    calculated_entry = calculate_time(8, 9);
  
  //Use maxDays to determine if the days are valid for the month value provided
  if(!(calculated_entry <= maxDays && calculated_entry >= 1)){
    return 2;
  } else {
    sDate->Date = calculated_entry;
  }

  sDate->WeekDay = SakamotoAlgo(*sDate);

  calculated_entry = calculate_time(11, 12);

  if(!(calculated_entry <= 23 && calculated_entry >= 0)){
    return 2;
  } else {
    sTime->Hours = calculated_entry;
  }

  calculated_entry = calculate_time(14, 15);

  if(!(calculated_entry <= 59 && calculated_entry >= 0)){
    return 2;
  } else {
    sTime->Minutes = calculated_entry;
  }

  calculated_entry = calculate_time(17, 18);

  if(!(calculated_entry <= 59 && calculated_entry >= 0)){
    return 2;
  } else {
    sTime->Seconds = calculated_entry;
  }

  return 0;

}

/*A function to set the RTC time. Returns 0 on success, 1 for formatted string errors, 2 for errors attempting HAL_SET time or date*/
uint8_t RTC_SetTime(void){

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  uint8_t RTC_checkTime_return = RTC_checkTimeInput((const char *)usartMessage, &sDate, &sTime);

  if(RTC_checkTime_return){
    //If RTC_checkTimeInput returned 1, then a formatting error was encountered...pass it along by also returning 1
    if (RTC_checkTime_return == 1){
      return 1;
    } else if (RTC_checkTime_return == 2) { // If it returned 2, pass along an invlaid date/time error by also returning 2
      return 2;
    } else { // Unrecognized return code received
      return -1;
    }
  } 

  //If the program proceeds here, should be formatted correctly. Now to set the time and check the return type.
  HAL_StatusTypeDef ret = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  if(ret != HAL_OK){
    return 3;
  }
  ret = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  if(ret != HAL_OK){
    return 3;
  }

  return 0;
}