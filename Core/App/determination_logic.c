#include "determination_logic.h"
#include "defines.h"
#include "error_codes.h"
#include "faults.h"
#include "logging.h"
#include "main.h"
#include "usb_comm.h"
#include <math.h>

extern UART_HandleTypeDef huart2;

int determineGreatestOutlier(float average, Sensor sensors[], float devOfTemp[]);

float findAverageValidTemp(Sensor sensors[])
{
    uint8_t validCount = 0;
    float tally = 0;

    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        if (!sensors[i].faults)
        {
            validCount++;
            tally += sensors[i].currTemp;
        }
    }

    if (!validCount)
    {
        return NAN;
    }

    return (tally / validCount);
}

uint8_t invalidateOutOfBounds(Sensor sensors[])
{
    uint8_t validCount = 0;
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        if (sensors[i].currTemp > UPPER_BOUND)
        {
            sensors[i].faults |= ABOVE_BOUNDS;
            logError(READ_ABOVE_BOUNDS, i);
        }
        else if (sensors[i].currTemp < LOWER_BOUND)
        {
            sensors[i].faults |= BELOW_BOUNDS;
            logError(READ_BELOW_BOUNDS, i);
        }
        else if (!sensors[i].faults)
        {
            validCount++;
        }
    }
    return validCount; // A return of 0 indicates that there are no more valid readings, otherwise
                       // there's at least one more valid reading
}

float determineTemp(Sensor sensors[])
{
    float devOfTemp[SENSOR_COUNT]; // Stores the deviations of each sensor's current temp reading
                                   // from the average of currently valid values, and stores 0 for
                                   // invalid values
    float tally = 0; // Stores the sum of currently considered valid temperature measurements
    uint8_t validReadingCount = 0;         // Stores the count of currently valid temp measurements
    uint8_t allRemainingReadingsValid = 0; // Determines when all remaining measurements are
                                           // considered valid (treated as a boolean variable)
    float average = 0; // Stores the current average of the remaining temp values
    float display_temperature = NAN;

    int validIndex = -1; // Stores at least one valid index, if there are any...

    /*Determine number of valid, communicating sensors, and find their average temperature...*/
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        if (sensors[i].faults)
        {
            continue;
        }
        else
        {
            validReadingCount++;
            tally += sensors[i].currTemp;
            validIndex = i;
        }
    }

    /*if no valid measurements, display an error, and keep displayTemp as NAN...*/
    if (validReadingCount == 0)
    {
        logError(ALL_SENSOR_READS_MISSING, -1);
        return display_temperature;
    }

    /*If only one one currently valid temperature measurement is available, use that one...*/
    if (validReadingCount == 1)
    {
        display_temperature = sensors[validIndex].currTemp;
        invalidateOutOfBounds(sensors);
        return display_temperature;
    }

    /*Averages the valid temp measurements*/
    average = tally / validReadingCount;

    uint8_t biggestOutlierIndex =
        0; // Stores the index of the sensor with the highest variation from the average
    uint8_t orderInvalidated[validReadingCount]; // Stores the indices of the sensors invalidated,
                                                 // with the most recently invalidated earlier in
                                                 // the list (all other entries = -1)
    // Initialize the above array:
    for (int i = 0; i < validReadingCount; i++)
    {
        orderInvalidated[i] = -1;
    }

    while (!allRemainingReadingsValid)
    {
        allRemainingReadingsValid = 1;
        biggestOutlierIndex = determineGreatestOutlier(average, sensors, devOfTemp);
        /*If the temp with the biggest deviation is an outlier...*/
        if (fabsf(devOfTemp[biggestOutlierIndex]) >= DISAGREE_THRESHOLD)
        {
            /*Set the deviation to 0 so it no longer has the biggest deviation and no longer factors
             * into calculations...*/
            devOfTemp[biggestOutlierIndex] = 0;
            /*Recalculate the average temp without the reading just marked invalid*/
            average = ((average * validReadingCount) - sensors[biggestOutlierIndex].currTemp) /
                      (validReadingCount - 1);
            /*Mark the sensor's value as faulty by outlier*/
            sensors[biggestOutlierIndex].faults |= IS_OUTLIER;
            logError(READ_MARKED_AS_OUTLIER, biggestOutlierIndex);
            validReadingCount--;
            orderInvalidated[validReadingCount] = biggestOutlierIndex;
            /*If there is only one reading left standing after the second to last one was marked as
             * an 'outlier'...*/
            if ((validReadingCount == 1) && (SENSOR_COUNT > 1))
            {
                /*...then all of the temperature readings are "outliers" of each other...*/
                biggestOutlierIndex = 0;
                /*Find the last, unmarked reading...*/
                while (!devOfTemp[biggestOutlierIndex])
                {
                    biggestOutlierIndex++;
                }
                /*...and also mark it as an outlier*/
                sensors[biggestOutlierIndex].faults =
                    (sensors[biggestOutlierIndex].faults | IS_OUTLIER);
                /*Store it as the last value 'invalidated' as an outlier...*/
                orderInvalidated[0] = biggestOutlierIndex;
                /*Print a final error message...*/
                logError(READ_MARKED_AS_OUTLIER, biggestOutlierIndex);
                /*...including an EXTRA one declaring that all sensor readings were marked invalid*/
                logError(ALL_READS_MARKED_INVALID, biggestOutlierIndex);
                /*Mark all OOB (Out of Bounds) readings*/
                invalidateOutOfBounds(sensors);
                /*DESIGN CHOICE...display (as the display temperature) the last value "invalidated"
                 * that is within the sensor's specified operating range*/
                for (int i = 0; i < sizeof(orderInvalidated); i++)
                {
                    if ((sensors[orderInvalidated[i]].faults & ABOVE_BOUNDS ||
                         sensors[orderInvalidated[i]].faults & BELOW_BOUNDS))
                    {
                        continue;
                    }
                    else
                    {
                        display_temperature = sensors[orderInvalidated[i]].currTemp;
                        return display_temperature;
                    }
                }

                /*If all of the communicating sensor measurements are either above or below the
                 * specified operating range, return the last one 'invalidated'*/
                display_temperature = sensors[biggestOutlierIndex].currTemp;
                return display_temperature;
            }
            allRemainingReadingsValid = 0;
        }
    }

    /*All Outliers should have been removed. Check for readings outside of the documented sensor
     * range bounds...*/
    if (invalidateOutOfBounds(sensors))
    { // If valid readings remain after invalidating out of bounds readings (return != 0)...
        // ...then return the average of all the still-valid sensors reading measurements
        display_temperature = findAverageValidTemp(sensors);
        return display_temperature;
    }

    /*Otherwise, log that all reads were invalidated, and keep the same display temp as before the
     * OOB measurements were 'invalidated'*/
    logError(ALL_READS_MARKED_INVALID, -1);
    display_temperature = average;
    return display_temperature;
}

int determineGreatestOutlier(float average, Sensor sensors[], float devOfTemp[])
{
    uint8_t biggestIndex =
        0; // Stores the index of the sensor with the highest variation from the average
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
        if (sensors[i].faults)
        {
            devOfTemp[i] = 0;
        }
        else
        {
            devOfTemp[i] = fabsf(average - sensors[i].currTemp);
            if (devOfTemp[biggestIndex] < devOfTemp[i])
            {
                biggestIndex = i;
            }
        }
    }
    return biggestIndex;
}