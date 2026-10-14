#include "defines.h"
#include "interrupt_handlers.h"
#include "unity.h"

#include "string.h"

// Variables normally defined in main.c and interrupts.c
uint8_t setTime = 0;
volatile uint8_t fullMessageReceived = 0;
volatile uint8_t usartMessage[BUFFER_SIZE] = {0};

// Variables for spying on usb_print
static char lastPrintedMessage[BUFFER_SIZE];
static int printCallCount = 0;

// Spy variables for the logError function
extern int logErrorCallCount;
extern int lastLoggedError;
extern int lastLoggedOptionalInt;

// Fake USB Transmission
void usb_print(const char *message)
{
    printCallCount++;
    snprintf(lastPrintedMessage, sizeof(lastPrintedMessage), "%s", message);
}

// Variables for spying on RTC_SetTime
uint8_t rtcSetTimeCallCount = 0;
uint8_t rtcSetTimeStubReturn = 0;

// Stub for RTC_SetTime (needed for linking)
int8_t RTC_SetTime(void)
{
    rtcSetTimeCallCount++;
    return rtcSetTimeStubReturn;
}

void test_usart_setTimeCommandAccepted(void)
{

    // Arrange
    setTime = 0;
    fullMessageReceived = 1;
    printCallCount = 0;
    lastPrintedMessage[0] = '\0';

    memcpy((void *)usartMessage, "SET_TIME", sizeof("SET_TIME"));

    // Act
    usartMessageHandler();

    // Assert
    TEST_ASSERT_TRUE(setTime);
    TEST_ASSERT_FALSE(fullMessageReceived);
    TEST_ASSERT_EQUAL(1, printCallCount);
    TEST_ASSERT_EQUAL_STRING("SEND_TIME\r\n", lastPrintedMessage);
}

void test_usart_incompleteMessageIgnored(void)
{
    // Arrange
    setTime = 0;
    fullMessageReceived = 0;
    printCallCount = 0;
    lastPrintedMessage[0] = '\0';

    memcpy((void *)usartMessage, "SET_TIME", sizeof("SET_TIME"));

    // Act
    usartMessageHandler();

    // Assert
    TEST_ASSERT_FALSE(setTime);
    TEST_ASSERT_FALSE(fullMessageReceived);
    TEST_ASSERT_EQUAL(0, printCallCount);
}

void test_usart_unrecognizedMessageRejected(void)
{
    // Arrange
    setTime = 0;
    fullMessageReceived = 1;
    printCallCount = 0;
    lastPrintedMessage[0] = '\0';

    logErrorCallCount = 0;
    lastLoggedError = -1;
    lastLoggedOptionalInt = 0;

    memcpy((void *)usartMessage, "SET_TIME ", sizeof("SET_TIME "));

    usartMessageHandler();

    TEST_ASSERT_FALSE(setTime);
    TEST_ASSERT_FALSE(fullMessageReceived);
    TEST_ASSERT_EQUAL_UINT8(1, logErrorCallCount);
    TEST_ASSERT_EQUAL_INT(500, lastLoggedError);
    TEST_ASSERT_EQUAL_INT(-1, lastLoggedOptionalInt);
}

void test_usart_validDateTimeSettingSuccessful(void)
{
    setTime = 1;
    fullMessageReceived = 1;
    rtcSetTimeCallCount = 0;
    rtcSetTimeStubReturn = 0;
    printCallCount = 0;
    lastPrintedMessage[0] = '\0';
    logErrorCallCount = 0;

    memcpy((void *)usartMessage, "2026/10/12 09:30:00", sizeof("2026/10/12 09:30:00"));

    usartMessageHandler();

    TEST_ASSERT_EQUAL_UINT8(1, rtcSetTimeCallCount);
    TEST_ASSERT_EQUAL_UINT8(0, setTime);
    TEST_ASSERT_EQUAL_UINT8(0, fullMessageReceived);
    TEST_ASSERT_EQUAL_INT(0, printCallCount);
    TEST_ASSERT_EQUAL_INT(0, logErrorCallCount);
}