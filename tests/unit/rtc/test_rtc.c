#include "defines.h"
#include "rtc.h"
#include "stm32l4xx_hal.h"
#include "unity.h"
#include <string.h>

extern int fakeRTCSetTimeCallCount;
extern int fakeRTCSetDateCallCount;

extern RTC_TimeTypeDef fakeLastTime;
extern RTC_DateTypeDef fakeLastDate;

extern HAL_StatusTypeDef fakeRTCSetTimeReturn;
extern HAL_StatusTypeDef fakeRTCSetDateReturn;

volatile uint8_t usartMessage[BUFFER_SIZE] = {0};

void setUsartMessage(const char *message)
{
    memcpy((void *)usartMessage, message, strlen(message) + 1);
}

/*TESTS FOR VALID DATE/TIME SETTING & HAL CODE FAILURES*/
void test_rtc_validDateTimeSetting(void)
{
    setUsartMessage("2026/10/12 09:30:59");
    TEST_ASSERT_EQUAL_STRING("2026/10/12 09:30:59", (const char *)usartMessage);

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(9, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(30, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(59, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(26, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(10, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(12, fakeLastDate.Date);
}

void test_rtc_HALSetTimeFailure(void)
{
    setUsartMessage("2226/10/12 09:30:59");
    fakeRTCSetTimeReturn = HAL_ERROR;

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(3, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT16(20, current_century);
}

void test_rtc_HALSetDateFailure(void)
{
    setUsartMessage("2126/10/12 09:30:59");
    fakeRTCSetDateReturn = HAL_ERROR;

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(3, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT16(20, current_century);
}

/*TEST SAKAMOTO ALGORITHM RESULTS*/
void test_rtc_sundayWeekdayCalculatedCorrectly(void)
{
    setUsartMessage("2026/10/11 12:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(7, fakeLastDate.WeekDay);
}

void test_rtc_leapCenturyWeekdayCalculatedCorrectly(void)
{
    setUsartMessage("2100/10/11 12:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_UINT8(1, fakeLastDate.WeekDay);
}

/*TESTS FOR DIFFERENT MALFORMED/INVALID DATE/TIME FORMATTING*/
void test_rtc_invalidDateDelimiterFormatting(void)
{
    setUsartMessage("2026-10/12 09:30:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_invalidTimeDelimiterFormatting(void)
{
    setUsartMessage("2026/10/12 09-30-59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_invalidSpaceBetweenFormatting(void)
{
    setUsartMessage("2026/10/12-09:30:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_invalidDateYearFormatting(void)
{
    setUsartMessage("26/10/12 09:30:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_invalidDateTimeFormattingTrailingSpace(void)
{
    setUsartMessage("2026/10/12 09:30:59 ");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_invalidDateTimeMissingSpaceBetweenFormatting(void)
{
    setUsartMessage("2026/10/1209:30:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_invalidDateFormattingLetter(void)
{
    setUsartMessage("2o26/10/12 09:30:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_invalidTimeFormattingLetter(void)
{
    setUsartMessage("2026/10/12 09:3o:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(1, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

/*TESTS FOR VALID/INVALID BOUNDARY CONDITIONS OF DATE/TIME VALUES*/

void test_rtc_minimumAllowableDateTimeValueAccepted(void)
{
    setUsartMessage("0000/01/01 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(1, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(1, fakeLastDate.Date);
    TEST_ASSERT_EQUAL_UINT8(6, fakeLastDate.WeekDay);
    TEST_ASSERT_EQUAL_UINT16(0, current_century);
}

void test_rtc_maximumAllowableDateTimeValueAccepted(void)
{
    setUsartMessage("9999/12/31 23:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(23, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(59, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(59, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(99, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(12, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(31, fakeLastDate.Date);
    TEST_ASSERT_EQUAL_UINT16(99, current_century);
}

void test_rtc_aboveValidTimeSecondBoundRejected(void)
{
    setUsartMessage("2026/10/12 09:59:60");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_aboveValidTimeMinuteBoundRejected(void)
{
    setUsartMessage("2026/10/12 01:60:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_aboveValidTimeHourBoundRejected(void)
{
    setUsartMessage("2026/10/12 24:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_aboveValidDateMonthBoundRejected(void)
{
    setUsartMessage("2026/13/12 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_belowValidDateDateBoundRejected(void)
{
    setUsartMessage("0000/01/00 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_belowValidDateMonthBoundRejected(void)
{
    setUsartMessage("0000/00/01 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableJanuaryDateValueAccepted(void)
{
    setUsartMessage("0000/01/31 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(1, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(31, fakeLastDate.Date);
}

void test_rtc_aboveValidJanuaryDateBoundRejected(void)
{
    setUsartMessage("2026/01/32 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableFebruaryNotLeapYearDateValueAccepted(void)
{
    setUsartMessage("0026/02/28 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(26, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(2, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(28, fakeLastDate.Date);
}

void test_rtc_aboveValidFebruaryNotLeapYearDateBoundRejected(void)
{
    setUsartMessage("2026/02/29 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableFebruaryLeapYearDateValueAccepted(void)
{
    setUsartMessage("2028/02/29 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(28, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(2, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(29, fakeLastDate.Date);
}

void test_rtc_aboveValidFebruaryLeapYearDateBoundRejected(void)
{
    setUsartMessage("2028/02/30 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableMarchDateValueAccepted(void)
{
    setUsartMessage("0000/03/31 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(3, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(31, fakeLastDate.Date);
}

void test_rtc_aboveValidMarchDateBoundRejected(void)
{
    setUsartMessage("2026/03/32 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableAprilDateValueAccepted(void)
{
    setUsartMessage("0000/04/30 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(4, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(30, fakeLastDate.Date);
}

void test_rtc_aboveValidAprilDateBoundRejected(void)
{
    setUsartMessage("2026/04/31 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableMayDateValueAccepted(void)
{
    setUsartMessage("0000/05/31 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(5, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(31, fakeLastDate.Date);
}

void test_rtc_aboveValidMayDateBoundRejected(void)
{
    setUsartMessage("2026/05/32 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableJuneDateValueAccepted(void)
{
    setUsartMessage("0000/06/30 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(6, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(30, fakeLastDate.Date);
}

void test_rtc_aboveValidJuneDateBoundRejected(void)
{
    setUsartMessage("2026/06/31 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableJulyDateValueAccepted(void)
{
    setUsartMessage("0000/07/31 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(7, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(31, fakeLastDate.Date);
}

void test_rtc_aboveValidJulyDateBoundRejected(void)
{
    setUsartMessage("2026/07/32 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableAugustDateValueAccepted(void)
{
    setUsartMessage("0000/08/31 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(8, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(31, fakeLastDate.Date);
}

void test_rtc_aboveValidAugustDateBoundRejected(void)
{
    setUsartMessage("2026/08/32 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableSeptemberDateValueAccepted(void)
{
    setUsartMessage("0000/09/30 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(9, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(30, fakeLastDate.Date);
}

void test_rtc_aboveValidSeptemberDateBoundRejected(void)
{
    setUsartMessage("2026/09/31 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableOctoberDateValueAccepted(void)
{
    setUsartMessage("0000/10/31 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(10, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(31, fakeLastDate.Date);
}

void test_rtc_aboveValidOctoberDateBoundRejected(void)
{
    setUsartMessage("2026/10/32 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableNovemberDateValueAccepted(void)
{
    setUsartMessage("0000/11/30 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(11, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(30, fakeLastDate.Date);
}

void test_rtc_aboveValidNovemberDateBoundRejected(void)
{
    setUsartMessage("2026/11/31 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableDecemberDateValueAccepted(void)
{
    setUsartMessage("0000/12/31 00:00:00");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(12, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(31, fakeLastDate.Date);
}

void test_rtc_aboveValidDecemberDateBoundRejected(void)
{
    setUsartMessage("2026/12/32 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

/*TESTING LEAP YEAR DETERMINATION*/

void test_rtc_aboveValidFebruaryNotLeapCenturyDateBoundRejected(void)
{
    setUsartMessage("2100/02/29 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void test_rtc_maximumAllowableFebruaryLeapCenturyDateValueAccepted(void)
{
    setUsartMessage("2400/02/29 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(9, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(59, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(59, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(2, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(29, fakeLastDate.Date);
}

void test_rtc_maximumAllowableFebruaryLeapMilliniumDateValueAccepted(void)
{
    setUsartMessage("2000/02/29 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(0, result);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetTimeCallCount);
    TEST_ASSERT_EQUAL_INT(1, fakeRTCSetDateCallCount);
    TEST_ASSERT_EQUAL_UINT8(9, fakeLastTime.Hours);
    TEST_ASSERT_EQUAL_UINT8(59, fakeLastTime.Minutes);
    TEST_ASSERT_EQUAL_UINT8(59, fakeLastTime.Seconds);
    TEST_ASSERT_EQUAL_UINT8(0, fakeLastDate.Year);
    TEST_ASSERT_EQUAL_UINT8(2, fakeLastDate.Month);
    TEST_ASSERT_EQUAL_UINT8(29, fakeLastDate.Date);
}

void test_rtc_aboveValidFebruaryLeapCenturyDateBoundRejected(void)
{
    setUsartMessage("2400/02/30 09:59:59");

    uint8_t result = RTC_SetTime();

    TEST_ASSERT_EQUAL_UINT8(2, result);
    TEST_ASSERT_FALSE(fakeRTCSetTimeCallCount);
    TEST_ASSERT_FALSE(fakeRTCSetDateCallCount);
}

void setUp(void)
{
    current_century = 20;

    fakeRTCSetTimeCallCount = 0;
    fakeRTCSetDateCallCount = 0;

    fakeRTCSetTimeReturn = HAL_OK;
    fakeRTCSetDateReturn = HAL_OK;
}

void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_rtc_validDateTimeSetting);
    RUN_TEST(test_rtc_HALSetTimeFailure);
    RUN_TEST(test_rtc_HALSetDateFailure);
    RUN_TEST(test_rtc_sundayWeekdayCalculatedCorrectly);
    RUN_TEST(test_rtc_leapCenturyWeekdayCalculatedCorrectly);
    RUN_TEST(test_rtc_invalidDateDelimiterFormatting);
    RUN_TEST(test_rtc_invalidTimeDelimiterFormatting);
    RUN_TEST(test_rtc_invalidSpaceBetweenFormatting);
    RUN_TEST(test_rtc_invalidDateYearFormatting);
    RUN_TEST(test_rtc_invalidDateTimeFormattingTrailingSpace);
    RUN_TEST(test_rtc_invalidDateTimeMissingSpaceBetweenFormatting);
    RUN_TEST(test_rtc_invalidDateFormattingLetter);
    RUN_TEST(test_rtc_invalidTimeFormattingLetter);
    RUN_TEST(test_rtc_minimumAllowableDateTimeValueAccepted);
    RUN_TEST(test_rtc_maximumAllowableDateTimeValueAccepted);
    RUN_TEST(test_rtc_aboveValidTimeSecondBoundRejected);
    RUN_TEST(test_rtc_aboveValidTimeMinuteBoundRejected);
    RUN_TEST(test_rtc_aboveValidTimeHourBoundRejected);
    RUN_TEST(test_rtc_aboveValidDateMonthBoundRejected);
    RUN_TEST(test_rtc_belowValidDateDateBoundRejected);
    RUN_TEST(test_rtc_belowValidDateMonthBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableJanuaryDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidJanuaryDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableFebruaryNotLeapYearDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidFebruaryNotLeapYearDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableFebruaryLeapYearDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidFebruaryLeapYearDateBoundRejected);
    RUN_TEST(test_rtc_aboveValidFebruaryNotLeapCenturyDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableFebruaryLeapCenturyDateValueAccepted);
    RUN_TEST(test_rtc_maximumAllowableFebruaryLeapMilliniumDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidFebruaryLeapCenturyDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableMarchDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidMarchDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableAprilDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidAprilDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableMayDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidMayDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableJuneDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidJuneDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableJulyDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidJulyDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableAugustDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidAugustDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableSeptemberDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidSeptemberDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableOctoberDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidOctoberDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableNovemberDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidNovemberDateBoundRejected);
    RUN_TEST(test_rtc_maximumAllowableDecemberDateValueAccepted);
    RUN_TEST(test_rtc_aboveValidDecemberDateBoundRejected);

    return UNITY_END();
}