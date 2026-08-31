#include "determination_logic.h"
#include "usb_comm.h"
#include <math.h>
#include "main.h"
#include "faults.h"

extern int IN_DEVELOPMENT;

extern UART_HandleTypeDef huart2;
extern DISAGREE_THRESHOLD;
extern BUFFER_SIZE;
extern __uint8_t txBuf[];

int determineGreatestOutlier(float average, Sensor sensors[], float devOfTemp[]);
float determineTemp(float display_temperature, Sensor sensors[]);

float determineTemp(float display_temperature, Sensor sensors[]){
    float devOfTemp[SENSOR_COUNT]; //Stores the deviations from the average of currently valid values, and stores 0 for invalid values
    float tally = 0; //Stores the tally of temperature measurements
    __uint8_t validCount = 0; //Stores the count of currently valid temp measurements
    __uint8_t allValid = 0; //Determines when enough is enough, when all remaining measurements are considered valid (treated as a boolean variable)
    float average = 0; //Stores the current average of the remaining temp values
    float display_temperature = NAN;

    int validIndex = -1; //Stores at least one valid index, if there are any...

    /*Determine number of valid, communicating sensors, and find their average temperature...*/
    for(int i = 0; i < SENSOR_COUNT; i++){
        if(sensors[i].faults){
        /****************************DELETE LATER************************************/
        if(IN_DEVELOPMENT){
            usb_printf_int("ERROR: No valid sensor %u reading found...\r\n", i);
        }
        /****************************DELETE LATER************************************/
        continue;
        }
        else{
        validCount++;
        tally+= sensors[i].currTemp;
        validIndex = i;
        }
    }

    /*if no valid measurements, display an error, and keep displayTemp as NAN...*/
    if(validIndex == -1){
        /*INSERT HERE IF YOU WANT AN ALERT FOR THIS CASE (E.G. BUZZER SOUND...)*/
        validCount = -1;
        return validCount;
    }

    /*If only one one currently valid temperature measurement being transmitted, use that one...*/
    if(validCount == 1){
        display_temperature = sensors[validIndex].currTemp;
        return validCount;
    }

    /*Averages the valid temp measurements*/
    average = tally/validCount;

    __uint8_t biggestIndex = 0; //Stores the index of the sensor with the highest variation from the average
    while(!allValid){
        allValid = 1;
        biggestIndex = determineGreatestOutlier(average, sensors, devOfTemp);
        /*If the temp with the biggest deviation is an outlier...*/
        if(devOfTemp[biggestIndex] >= DISAGREE_THRESHOLD || devOfTemp[biggestIndex] <= (-1 * DISAGREE_THRESHOLD)){
            /*Set the deviation to 0 so it no longer is the biggest and no longer factors into calculations...*/
            devOfTemp[biggestIndex] = 0;
            /*Recalculate the average temp without the reading just marked invalid*/
            average = ((average*validCount) - sensors[biggestIndex].currTemp)/(validCount-1);
            /*Mark the sensor's value as faulty by outlier*/
            sensors[biggestIndex].faults |= IS_OUTLIER;
            usb_printf_int("Sensor %u reading marked invalid as outlier\r\n", biggestIndex);
            validCount--;
            /*If there is only one reading left standing, after the second to last one was marked an 'outlier'...*/
            if((validCount == 1) && (SENSOR_COUNT > 1)){
                /*All of the temperature readings are "outliers" of each other...*/
                biggestIndex = 0;
                /*Find the last, unmarked reading...*/
                while(!devOfTemp[biggestIndex]){
                biggestIndex++;
                }
                /*...and mark it as also an invalid outlier*/
                sensors[biggestIndex].faults = (sensors[biggestIndex].faults | IS_OUTLIER);
                if(IN_DEVELOPMENT){
                    /*Print a final error message...*/
                    usb_printf_int("Sensor %u reading marked invalid as outlier\r\n", biggestIndex);
                    /*...including an EXTRA one declaring that all sensor readings were marked invalid*/
                }
                usb_printf_int("WARNING: All sensor readings marked as outliers...selecting value from sensor: %u \r\n", biggestIndex);
                /*DESIGN CHOICE...display (as the display temperature) the last value invalidated*/
                display_temperature = sensors[biggestIndex].currTemp;
                validCount = 0;
                return validCount;
            }
            allValid = 0;
        }
    }
    /*If more than one valid sensor reading, return the average.*/
    display_temperature = average;
    return validCount;
}


int determineGreatestOutlier(float average, Sensor sensors[], float devOfTemp[]){
    __uint8_t biggestIndex = 0; //Stores the index of the sensor with the highest variation from the average
    for(int i = 0; i < SENSOR_COUNT; i++){
        if(sensors[i].faults){devOfTemp[i] = 0;}
        else{
            int sign = 1;
            if(average - sensors[i].currTemp < 0){
            sign = -1;
            }
            devOfTemp[i] = (average - sensors[i].currTemp) * sign;
            if(devOfTemp[biggestIndex] < devOfTemp[i]){
            biggestIndex = i;
            }
        }
    }
    return biggestIndex;
}