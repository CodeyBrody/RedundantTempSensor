#include "defines.h"
#include "logging.h"
#include "stdio.h"

int logErrorCallCount = 0;
int lastLoggedError = -1;
int lastLoggedOptionalInt = 0;

int fake_LogError_Codes[4] = {-1};
int fake_LogError_SensorNums[4] = {-2};

void logError(int Error, int optionalInt)
{
    lastLoggedError = Error;
    lastLoggedOptionalInt = optionalInt;
    if (logErrorCallCount < 4)
    {
        fake_LogError_Codes[logErrorCallCount] = lastLoggedError;
        fake_LogError_SensorNums[logErrorCallCount] = lastLoggedOptionalInt;
    }
    logErrorCallCount++;
}