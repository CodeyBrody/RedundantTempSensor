#include "hardware.h"

const uint8_t TEMP_REGISTER_ADDRESS = 0;
const float TEMP_CONVERSION_VALUE = 0.0625;

/*Addresses being used by the sensors*/
const uint8_t SENSOR_ADDRESSES[] = {(0x48 << 1), (0x49 << 1), (0x4A << 1)};

/*Assign each LED in use to a specific shift register output*/
/*DEFINE SHIFT REGISTER OUTPUT NUMBERS FOR EACH LED IN USE HERE:👇*/
const int SENSOR_1_LED_G = 0;
const int SENSOR_1_LED_Y = 1;
const int SENSOR_1_LED_R = 2;
const int SENSOR_2_LED_G = 3;
const int SENSOR_2_LED_Y = 4;
const int SENSOR_2_LED_R = 5;
const int SENSOR_3_LED_G = 6;
const int SENSOR_3_LED_Y = 7;
const int SENSOR_3_LED_R = 8;

/*Pins for sensor RGY LEDs (depends on how many sensors are in use...in later code the index of
 * these arrays is to determine which sensor is using which LED pin)*/
const int REDLeds[SENSOR_COUNT] = {SENSOR_1_LED_R, SENSOR_2_LED_R, SENSOR_3_LED_R};
const int YELLOWLeds[SENSOR_COUNT] = {SENSOR_1_LED_Y, SENSOR_2_LED_Y, SENSOR_3_LED_Y};
const int GREENLeds[SENSOR_COUNT] = {SENSOR_1_LED_G, SENSOR_2_LED_G, SENSOR_3_LED_G};