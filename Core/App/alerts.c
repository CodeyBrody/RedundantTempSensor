#include "alerts.h"
#include "defines.h"
#include "hardware.h"
#include "main.h"


#define BEEP_LENGTH 500 // The length of a buzzer beep (in ms)

static uint16_t LEDVals =
    0; // Initializes a variable that is used to control the shift register outputs
static uint8_t activeBuzzer = 0; // Keeps track of whether the buzzer is currently, well, buzzing
static uint32_t buzzerStartTime = 0;
static uint32_t buzzerDuration = 0;

typedef enum
{
    BEEP,
    CONSTANT
} buzzType;

void setSensorLeds(Sensor sensors[SENSOR_COUNT]);

void setLEDValue(uint8_t LEDNumber, uint16_t LEDValue);

void shiftRegWrite(const uint16_t LEDVals);

void setSensorLeds(Sensor sensors[SENSOR_COUNT])
{
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        uint8_t flags = sensors[i].faults;
        if (flags)
        { /*If not valid...*/
            if (flags & COMM_FAULT)
            { /*If Failure, set LED to red...*/
                setLEDValue(sensors[i].RYG.RED, 1);
                setLEDValue(sensors[i].RYG.YELLOW, 0);
                setLEDValue(sensors[i].RYG.GREEN, 0);
            }
            else
            { /*...else if invalid, set the yellow LED...*/
                setLEDValue(sensors[i].RYG.RED, 0);
                setLEDValue(sensors[i].RYG.YELLOW, 1);
                setLEDValue(sensors[i].RYG.GREEN, 0);
            } /*...and if it is valid, set LED to green.*/
        }
        else
        {
            setLEDValue(sensors[i].RYG.RED, 0);
            setLEDValue(sensors[i].RYG.YELLOW, 0);
            setLEDValue(sensors[i].RYG.GREEN, 1);
        }
    }
    shiftRegWrite(LEDVals);
}

void setLEDValue(uint8_t LEDNumber, uint16_t LEDValue)
{
    if (LEDValue)
    {
        LEDVals |= (1 << LEDNumber);
    }
    else
    {
        LEDVals &= ~(1 << LEDNumber);
    }
    return;
}

void shiftRegWrite(const uint16_t LEDVals)
{
    HAL_GPIO_WritePin(LATCH_GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, (uint8_t *)&LEDVals, 1, SPI_TIMEOUT_MS);
    HAL_GPIO_WritePin(LATCH_GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);
}

void buzzerOn(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
}

void buzzerOff(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
}

void activateBuzzer(buzzType duration /*The duration of the desired buzz (in ms)*/)
{
    activeBuzzer = 1;
    buzzerStartTime = HAL_GetTick();
    if (duration == BEEP)
    {
        buzzerDuration = BEEP_LENGTH;
    }
    else
    {
        buzzerDuration = 20000 * REQUESTED_SENSOR_SAMPLING_INTERVAL_SEC;
    }
    buzzerOn();
}

void setBuzzer(Sensor sensors[SENSOR_COUNT])
{
    uint8_t soundBuzzer = 0;
    uint8_t missingCount = 0;
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        if (sensors[i].faults)
        {
            if (sensors[i].faults & COMM_FAULT)
            {
                missingCount++;
            }
            if (sensors[i].lastFaults == sensors[i].faults)
            {
                continue;
            }
            else
            {
                soundBuzzer = 1;
            }
        }
    }
    if (missingCount == SENSOR_COUNT)
    {
        soundBuzzer = 2;
    }
    switch (soundBuzzer)
    {
        case 1:
            activateBuzzer(BEEP);
            break;
        case 2:
            activateBuzzer(CONSTANT);
            break;
        case 0:
            break;
        default:
            break;
    }
    return;
}

void updateBuzzer(void)
{
    if (activeBuzzer && ((HAL_GetTick() - buzzerStartTime) >= buzzerDuration))
    {
        activeBuzzer = 0;
        buzzerOff();
    }
}

void setAlerts(Sensor sensors[])
{
    setSensorLeds(sensors);
    setBuzzer(sensors);
    return;
}