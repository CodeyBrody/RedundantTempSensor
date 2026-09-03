#ifndef ERROR_CODES_H
#define ERROR_CODES_H

/*ERROR CODES*/

/*SENSOR READ ERRORS*/
/*Missing Sensors*/
#define MISSING_SENSORS 100 //Unable to discover the number of sensors expected
/*Missing Sensor Readings*/
#define ALL_SENSOR_READS_MISSING 200 //All Sensor Readings Missing
#define SENSOR_READ_MISSING 201 //Sensor Reading Missing

/*VALIDATION ERRORS*/
#define ALL_READS_MARKED_INVALID 300 //All sensor readings marked as invalid
/*Specific validation errors*/
#define READ_ABOVE_BOUNDS 301 //Sensor reading marked invalid for being above the documented valid sensor temperature reading range
#define READ_BELOW_BOUNDS 302 //Sensor reading marked invalid for being below the documented valid sensor temperature reading range
#define READ_MARKED_AS_OUTLIER 303 //Sensor reading marked invalid as an outlier from the other (currently considered valid) sensor reads

/*LED ERRORS*/
#define NOT_ENOUGH_LEDS 400 //Insufficient LEDs for the number of sensors utilized

#endif