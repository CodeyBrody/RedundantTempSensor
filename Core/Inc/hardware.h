#ifndef HARDWARE_H
#define HARDWARE_H
#include <stdint.h>

#define SENSOR_COUNT 3

/*Specified Temperature Sensor Operating Range*/
#define UPPER_BOUND 125.0
#define LOWER_BOUND -40.0

/*SPI2 Defines (for shift register communication)*/
#define LATCH_GPIO_PORT GPIOB
#define LATCH_PIN GPIO_PIN_4

/*Buzzer Defines (for communicating with the buzzer)*/
#define BUZZER_GPIO_PORT GPIOC
#define BUZZER_PIN GPIO_PIN_7

/*Assign each LED in use to a specific shift register output (define in hardware.c)*/
extern const int SENSOR_1_LED_G;
extern const int SENSOR_1_LED_Y;
extern const int SENSOR_1_LED_R;
extern const int SENSOR_2_LED_G;
extern const int SENSOR_2_LED_Y;
extern const int SENSOR_2_LED_R;
extern const int SENSOR_3_LED_G;
extern const int SENSOR_3_LED_Y;
extern const int SENSOR_3_LED_R;

/*Arrays to hold shift register output numbers for different types of LEDs (index of array used to
 * determine which sensor is using which LED)*/
extern const int REDLeds[SENSOR_COUNT];
extern const int YELLOWLeds[SENSOR_COUNT];
extern const int GREENLeds[SENSOR_COUNT];

/*Create an RGB struct to hold the pin numbers controlling each RGB*/
struct RYGLed
{
    uint16_t RED;
    uint16_t YELLOW;
    uint16_t GREEN;
};

#endif