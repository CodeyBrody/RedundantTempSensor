#include "hardware.h"

const uint8_t TEMP_REGISTER_ADDRESS = 0;
const float TEMP_CONVERSION_VAL = 0.0625;

/*Addresses being used by the sensors*/
const uint8_t SENSOR_ADDRESSES[] = {(0x48<<1), (0x49<<1), (0x4A<<1)};

/*DEFINE LED NUMBERS TO BE USED HERE:👇*/
const int SENSOR_1_LED_G = 0; 
const int SENSOR_1_LED_Y = 1;
const int SENSOR_1_LED_R = 2;
const int SENSOR_2_LED_G = 3;
const int SENSOR_2_LED_Y = 4;
const int SENSOR_2_LED_R = 5;
const int SENSOR_3_LED_G = 6;
const int SENSOR_3_LED_Y = 7;
const int SENSOR_3_LED_R = 8;
// const int MODE_LED_PIN_G = 9;
// const int MODE_LED_PIN_Y = 10;
// const int MODE_LED_PIN_R = 11;
// const int MODE_LED_PIN_B = 12;

/*Pins for sensor RGY LEDs (depends on how many sensors in use...index of array used to determine which sensor is using which LED pin)*/
const int REDLeds[SENSOR_COUNT] = {SENSOR_1_LED_R, SENSOR_2_LED_R, SENSOR_3_LED_R};
const int YELLOWLeds[SENSOR_COUNT] = {SENSOR_1_LED_Y, SENSOR_2_LED_Y, SENSOR_3_LED_Y};
const int GREENLeds[SENSOR_COUNT] = {SENSOR_1_LED_G, SENSOR_2_LED_G, SENSOR_3_LED_G};