#ifndef HARDWARE_H
#define HARDWARE_H
#include <stdint.h>

#define SENSOR_COUNT 3

/*SPI2 Defines (for communicating with shift register)*/
#define LATCH_GPIO_PORT     GPIOB
#define LATCH_PIN           GPIO_PIN_4

/*DEFINE LED NUMBERS TO BE USED IN HARDWARE.C*/
extern const int SENSOR_1_LED_G; 
extern const int SENSOR_1_LED_Y;
extern const int SENSOR_1_LED_R;
extern const int SENSOR_2_LED_G;
extern const int SENSOR_2_LED_Y;
extern const int SENSOR_2_LED_R;
extern const int SENSOR_3_LED_G;
extern const int SENSOR_3_LED_Y;
extern const int SENSOR_3_LED_R;

/*Pins for sensor RGY LEDs (depends on how many sensors in use...index of array used to determine which sensor is using which LED pin)*/
extern const int REDLeds[SENSOR_COUNT];
extern const int YELLOWLeds[SENSOR_COUNT];
extern const int GREENLeds[SENSOR_COUNT];

/*Create an RGB struct to hold the pin numbers controlling each RGB*/
struct RYGLed {
  uint16_t RED;
  uint16_t YELLOW;
  uint16_t GREEN;
};

#endif