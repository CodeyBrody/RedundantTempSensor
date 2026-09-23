#include "alerts.h"
#include "defines.h"
#include "hardware.h"
#include "main.h"

#define BEEP_LENGTH 500 // The length of a buzzer beep (in ms)

extern SPI_HandleTypeDef hspi1;

static uint16_t LEDVals =
    0; // Initializes a variable that is used to control the shift register outputs
static uint8_t activeBuzzer = 0; // Keeps track of whether the buzzer is currently, well, buzzing
static uint32_t buzzerStartTime = 0;
static uint32_t buzzerDuration = 0;

// The different possible types of 'buzzes' that the buzzer can make 👇
typedef enum
{
    BEEP,
    CONSTANT
} buzzType;

void setSensorLeds(Sensor sensors[SENSOR_COUNT]);

void setLEDValue(uint8_t LEDNumber, uint8_t LEDValue);

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
    // Set the LED values as determined above.
    shiftRegWrite(LEDVals);
}

/*Sets the value of a specific LED (indicated by "LEDNumber") to the boolean value indicated by
 * "LEDValue"*/
void setLEDValue(uint8_t LEDNumber, uint8_t LEDValue)
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
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

void buzzerOff(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
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
        /*The following code sets the buzzer duration to 2x the requested sensor sampling interval,
         * which should cause the buzzer to continuously sound until the system reaches a state
         * where the buzzer is no longer meant to be constantly sounding*/
        buzzerDuration = 20000 * REQUESTED_SENSOR_SAMPLING_INTERVAL_SEC;
    }
    buzzerOn();
}

/*This function sets the buzzer (activatiing or deactivating it for the appropriate amount of time)
 * based on the fault values of the sensors in the `sensors[]` array*/
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