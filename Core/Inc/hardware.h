#ifndef HARDWARE_H
#define HARDWARE_H

#define SENSOR_COUNT 3

/*SPI2 Defines (for communicating with shift register)*/
#define LATCH_GPIO_PORT     GPIOB
#define LATCH_PIN           GPIO_PIN_4

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
static const int REDLeds[SENSOR_COUNT] = {SENSOR_1_LED_R, SENSOR_2_LED_R, SENSOR_3_LED_R};
static const int YELLOWLeds[SENSOR_COUNT] = {SENSOR_1_LED_Y, SENSOR_2_LED_Y, SENSOR_3_LED_Y};
static const int GREENLeds[SENSOR_COUNT] = {SENSOR_1_LED_G, SENSOR_2_LED_G, SENSOR_3_LED_G};

/*Create an RGB struct to hold the pin numbers controlling each RGB*/
struct RYGLed {
  uint16_t RED;
  uint16_t YELLOW;
  uint16_t GREEN;
};

#endif