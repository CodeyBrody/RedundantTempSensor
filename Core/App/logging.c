#include "logging.h"
#include "error_codes.h"
#include "faults.h"
#include "stdio.h"
#include "stdlib.h"
#include "usb_comm.h"

uint8_t buf[5]; // Buffer to hold fault strings for the different temp sensor readings
/*ERROR MESSAGE FUNCTIONS*/
void MISSING_SENSORS_MESSAGE(int optionalInt);
void ALL_SENSOR_READS_MISSING_MESSAGE(void);
void SENSOR_READ_MISSING_MESSAGE(int optionalInt);
void ALL_READS_MARKED_INVALID_MESSAGE(int optionalInt);
void READ_ABOVE_BOUNDS_MESSAGE(int optionalInt);
void READ_BELOW_BOUNDS_MESSAGE(int optionalInt);
void READ_MARKED_AS_OUTLIER_MESSAGE(int optionalInt);
void NOT_ENOUGH_LEDS_MESSAGE(void);
void UNRECOGNIZED_COMMAND_RECEIVED_MESSAGE(void);
void RTC_FORMATTING_ERROR_MESSAGE(void);
void RTC_INVALID_DATETIME_ERROR_MESSAGE(void);
void RTC_SET_ERROR_MESSAGE(void);
void BUFFER_FULL_MESSAGE(void);
void BUFFER_OVERFLOW_MESSAGE(void);

/*Faults -> String Conversion Function*/
const char *fault_stringify(uint8_t fault_flag, uint8_t buf[]);

void logError(int Error, int optionalInt)
{
    /*Start off with the Error Header: 'E: ' */
    usb_print("E: ");

    /*Log the current timestamp*/
    logCurrentDateTime();
    usb_print_delimiter(" ");

    /*Print the error code*/
    usb_printf_int("[%u]", Error);
    usb_print_delimiter(" ");

    switch (Error)
    {
        case MISSING_SENSORS:
            MISSING_SENSORS_MESSAGE(optionalInt);
            break;
        case ALL_SENSOR_READS_MISSING:
            ALL_SENSOR_READS_MISSING_MESSAGE();
            break;
        case SENSOR_READ_MISSING:
            SENSOR_READ_MISSING_MESSAGE(optionalInt);
            break;
        case ALL_READS_MARKED_INVALID:
            ALL_READS_MARKED_INVALID_MESSAGE(optionalInt);
            break;
        case READ_ABOVE_BOUNDS:
            READ_ABOVE_BOUNDS_MESSAGE(optionalInt);
            break;
        case READ_BELOW_BOUNDS:
            READ_BELOW_BOUNDS_MESSAGE(optionalInt);
            break;
        case READ_MARKED_AS_OUTLIER:
            READ_MARKED_AS_OUTLIER_MESSAGE(optionalInt);
            break;
        case NOT_ENOUGH_LEDS:
            NOT_ENOUGH_LEDS_MESSAGE();
            break;
        case UNRECOGNIZED_COMMAND_RECEIVED:
            UNRECOGNIZED_COMMAND_RECEIVED_MESSAGE();
            break;
        case RTC_FORMATTING_ERROR:
            RTC_FORMATTING_ERROR_MESSAGE();
            break;
        case RTC_INVALID_DATETIME_ERROR:
            RTC_INVALID_DATETIME_ERROR_MESSAGE();
            break;
        case RTC_SET_ERROR:
            RTC_SET_ERROR_MESSAGE();
            break;
        case RX_BUFFER_FULL:
            BUFFER_FULL_MESSAGE();
            break;
        case RX_BUFFER_OVERFLOW:
            BUFFER_OVERFLOW_MESSAGE();
            break;
        default:
            usb_print("Error logged with unrecognized error code.");
            break;
    }

    usb_print("\r\n");

    return;
}

void logData(float displayTemp, Sensor sensors[])
{
    usb_print("D: ");

    logCurrentDateTime();
    usb_print_delimiter(", ");

    print_temp_c(displayTemp);

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        usb_print_delimiter(", ");
        print_temp_c(sensors[i].currTemp);
    }

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        usb_print_delimiter(", ");
        usb_print(fault_stringify(sensors[i].faults, buf));
    }

    usb_print("\r\n");
    return;
}

void logCurrentDateTime(void)
{
    uint8_t buf[25];

    // Create Date and Time Structs to get the store the date and time of different readings
    RTC_TimeTypeDef t;
    RTC_DateTypeDef d;

    HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);

    snprintf((char *)buf, sizeof(buf), "%02d/%02d/%04d %02d:%02d:%02d", d.Month, d.Date,
             2000 + d.Year, t.Hours, t.Minutes, t.Seconds);

    usb_print((const char *)buf);
    return;
}

const char *fault_stringify(uint8_t fault_flag, uint8_t buf[])
{
    /*Create a string with symbols indicating the faults of the sensor
     * => outlier, + => above sensor range, - => below sensor range, ` => communication error */
    uint8_t index = 0;
    if (fault_flag & COMM_FAULT)
    {
        buf[index] = '`';
        index++;
    }
    if (fault_flag & IS_OUTLIER)
    {
        buf[index] = '*';
        index++;
    }
    if (fault_flag & BELOW_BOUNDS)
    {
        buf[index] = '-';
        index++;
    }
    if (fault_flag & ABOVE_BOUNDS)
    {
        buf[index] = '+';
        index++;
    }
    buf[index] = '\0';
    return (char *)buf;
}

void MISSING_SENSORS_MESSAGE(int discoveredSensorCount)
{
    usb_print("Unable to discover the number of sensors expected.");
    usb_print_delimiter(" ");
    usb_printf_int("Proceeding with %u sensors found.", discoveredSensorCount);
    return;
}

void ALL_SENSOR_READS_MISSING_MESSAGE(void)
{
    usb_print("All Sensor Readings Missing.");
    return;
}

void SENSOR_READ_MISSING_MESSAGE(int sensorNum)
{
    usb_printf_int("Sensor %u Reading Missing.", sensorNum);
    return;
}

void ALL_READS_MARKED_INVALID_MESSAGE(int selectedReadSensorNum)
{
    usb_print("All sensor readings marked as invalid.");
    usb_print_delimiter(" ");
    if (selectedReadSensorNum == -1)
    { // This is used to indicate that an average temp is being displayed, not any one sensors'
      // values
        usb_print("Selecting average of non-outlier temperature readings.");
    }
    else
    {
        usb_printf_int("Selecting read from sensor %u.", selectedReadSensorNum);
    }
    return;
}

void READ_ABOVE_BOUNDS_MESSAGE(int sensorNum)
{
    usb_printf_int(
        "Sensor %u reading marked invalid for being above the specified sensor operating range.",
        sensorNum);
    return;
}

void READ_BELOW_BOUNDS_MESSAGE(int sensorNum)
{
    usb_printf_int(
        "Sensor %u reading marked invalid for being below the specified sensor operating range.",
        sensorNum);
    return;
}

void READ_MARKED_AS_OUTLIER_MESSAGE(int sensorNum)
{
    usb_printf_int("Sensor %u reading marked invalid as an outlier.", sensorNum);
    return;
}

void NOT_ENOUGH_LEDS_MESSAGE(void)
{
    usb_print("Insufficient LEDs for the number of sensors expected.");
    return;
}

void UNRECOGNIZED_COMMAND_RECEIVED_MESSAGE(void)
{
    usb_print("Unrecognized command received via USART.");
    usb_print_delimiter(" ");
    usb_print("Received: ");
    usb_print((const char *)usartMessage);
    return;
}

void RTC_FORMATTING_ERROR_MESSAGE(void)
{
    usb_print("Expected data to set RTC, but received data incorrectly formatted to do so.");
    usb_print_delimiter(" ");
    usb_print("Received: ");
    usb_print((const char *)usartMessage);
    return;
}

void RTC_INVALID_DATETIME_ERROR_MESSAGE(void)
{
    usb_print("An invalid date/time was received as input to set the RTC.");
    usb_print_delimiter(" ");
    usb_print("Received: ");
    usb_print((const char *)usartMessage);
    return;
}

void RTC_SET_ERROR_MESSAGE(void)
{
    usb_print("An error occurred while attempting to set the parsed RTC time or date.");
    return;
}

void BUFFER_FULL_MESSAGE(void)
{
    usb_print("The buffer is currently full of a command that has yet to be processed.");
    usb_print_delimiter(" ");
    usb_print("Please resend the prior message.");
}

void BUFFER_OVERFLOW_MESSAGE(void)
{
    usb_print("The rxBuf received a message or command with too many characters.");
    usb_print_delimiter(" ");
    usb_print("Received: ");
    usb_print((const char *)usartMessage);
    usb_print_delimiter(" ");
    usb_print("Clearing buffer."); // NOTICE the space in front of 'Clearing buffer'
    return;
}