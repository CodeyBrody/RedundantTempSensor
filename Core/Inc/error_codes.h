#ifndef ERROR_CODES_H
#define ERROR_CODES_H

/*ERROR CODES*/

/*SENSOR READ ERRORS*/
/*Missing Sensors*/
#define SENSORS_NOT_DETECTED 100 // Unable to discover the number of sensors expected
/*Missing Sensor Readings*/
#define ALL_SENSOR_READS_MISSING 200 // All Sensor Readings Missing
#define SENSOR_READ_MISSING 201      // Sensor Reading Missing

/*VALIDATION ERRORS*/
#define ALL_READS_MARKED_INVALID 300 // All sensor readings marked as invalid
/*Specific validation errors*/
#define READ_ABOVE_BOUNDS                                                                          \
    301 // Sensor reading marked invalid for being above the documented valid sensor temperature
        // reading range
#define READ_BELOW_BOUNDS                                                                          \
    302 // Sensor reading marked invalid for being below the documented valid sensor temperature
        // reading range
#define READ_MARKED_AS_OUTLIER                                                                     \
    303 // Sensor reading marked invalid as an outlier from the other (currently considered valid)
        // sensor reads

/*LED ERRORS*/
#define NOT_ENOUGH_LEDS 400 // Insufficient LEDs for the number of sensors utilized

/*USART COMM ERRORS*/
#define UNRECOGNIZED_COMMAND_RECEIVED 500 // Unrecognized command received via USART
#define RTC_FORMATTING_ERROR                                                                       \
    501 // Expected data to set RTC, but data received was incorrectly formatted to do so
#define RTC_INVALID_DATETIME_ERROR 502 // An invalid date was received as input to set the RTC
#define RTC_SET_ERROR 503  // An error occurred while attempting to set the parsed RTC time or date
#define RX_BUFFER_FULL 504 // The buffer is currently full of a command that has yet to be processed
#define RX_BUFFER_OVERFLOW 505 // The rxBuf received a message or command with too many characters

#endif