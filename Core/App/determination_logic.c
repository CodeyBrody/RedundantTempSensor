#include "determination_logic.h"
#include "usb_comm.h"
#include "logging.h"
#include <math.h>
#include "main.h"
#include "faults.h"
#include "defines.h"
#include "error_codes.h"

extern UART_HandleTypeDef huart2;

int determineGreatestOutlier(float average, Sensor sensors[], float devOfTemp[]);

float findAverageValidTemp(Sensor sensors[]){
    uint8_t validCount= 0;
    float tally = 0;

    for(int i = 0; i < SENSOR_COUNT; i++){
        if(!sensors[i].faults){
            validCount++;
            tally += sensors[i].currTemp;
        }
    }

    if(!validCount){
        return NAN;
    }

    return (tally / validCount); 
}

uint8_t invalidateOutOfBounds(Sensor sensors[]){
    uint8_t validCount = 0;
    for(int i = 0; i < SENSOR_COUNT; i++){
        if (sensors[i].currTemp > UPPER_BOUND){
            sensors[i].faults |= ABOVE_BOUNDS;
            logError(READ_ABOVE_BOUNDS, i);
        }
        else if(sensors[i].currTemp < LOWER_BOUND){
            sensors[i].faults |= BELOW_BOUNDS;
            logError(READ_BELOW_BOUNDS, i);
        }
        else if(!sensors[i].faults){
            validCount++;
        }
    }
    return validCount; // A return of 0 indicates that there are no more valid readings, otherwise there's at least one more valid reading
}

float determineTemp(Sensor sensors[]){
    float devOfTemp[SENSOR_COUNT]; //Stores the deviations from the average of currently valid values, and stores 0 for invalid values
    float tally = 0; //Stores the tally of temperature measurements
    uint8_t validCount = 0; //Stores the count of currently valid temp measurements
    uint8_t allValid = 0; //Determines when enough is enough, when all remaining measurements are considered valid (treated as a boolean variable)
    float average = 0; //Stores the current average of the remaining temp values
    float display_temperature = NAN;

    int validIndex = -1; //Stores at least one valid index, if there are any...

    /*Determine number of valid, communicating sensors, and find their average temperature...*/
    for(int i = 0; i < SENSOR_COUNT; i++){
        if(sensors[i].faults){
            continue;
        }
        else{
            validCount++;
            tally+= sensors[i].currTemp;
            validIndex = i;
        }
    }

    /*if no valid measurements, display an error, and keep displayTemp as NAN...*/
    if(validCount == 0){
        logError(ALL_SENSOR_READS_MISSING, -1);
        return display_temperature;
    }

    /*If only one one currently valid temperature measurement being transmitted, use that one...*/
    if(validCount == 1){
        display_temperature = sensors[validIndex].currTemp;
        invalidateOutOfBounds(sensors);
        return display_temperature;
    }

    /*Averages the valid temp measurements*/
    average = tally/validCount;

    uint8_t biggestIndex = 0; //Stores the index of the sensor with the highest variation from the average
    uint8_t orderInvalidated[validCount]; //Stores the indices of the sensors invalidated, with the most recently invalidated earlier in the list (all other entries = -1)
    //Initialize the above array:
    for(int i = 0; i < validCount; i++){
        orderInvalidated[i] = -1;
    }

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
            logError(READ_MARKED_AS_OUTLIER, biggestIndex);
            validCount--;
            orderInvalidated[validCount] = biggestIndex;
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
                /*Store it as the last value 'invalidated' as an outlier...*/
                orderInvalidated[0] = biggestIndex;
                /*Print a final error message...*/
                logError(READ_MARKED_AS_OUTLIER, biggestIndex);
                /*...including an EXTRA one declaring that all sensor readings were marked invalid*/
                logError(ALL_READS_MARKED_INVALID, biggestIndex);
                /*Mark all OOB (Out of Bounds) readings*/
                invalidateOutOfBounds(sensors);
                /*DESIGN CHOICE...display (as the display temperature) the last value "invalidated" that is within bounds..."*/
                for(int i = 0; i < sizeof(orderInvalidated); i++){
                    if((sensors[orderInvalidated[i]].faults & ABOVE_BOUNDS || sensors[orderInvalidated[i]].faults & ABOVE_BOUNDS)){
                        continue;
                    } else {
                        display_temperature = sensors[orderInvalidated[i]].currTemp;
                        return display_temperature;
                    }
                }

                /*If all of the communicating sensor measurements are either above or below the specified operating range, return the last one 'invalidated'*/
                display_temperature = sensors[biggestIndex].currTemp;
                return display_temperature;
            }
            allValid = 0;
        }
    }

    /*All Outliers should have been removed. Check for reads out of the documented sensor range bounds...*/
    if(invalidateOutOfBounds(sensors)){ // If there are valid readings after invalidating out of bound measurements (return != 0)...
        //Then return the average of all the still-valid sensors reading measurements
        display_temperature = findAverageValidTemp(sensors);
        return display_temperature;
    }

    /*Otherwise, log that all reads were invalidated, and keep the same display temp as before the OOB measurements were 'invalidated'*/
    logError(ALL_READS_MARKED_INVALID, -1);
    display_temperature = average;
    return display_temperature;
}

int determineGreatestOutlier(float average, Sensor sensors[], float devOfTemp[]){
    uint8_t biggestIndex = 0; //Stores the index of the sensor with the highest variation from the average
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