#include "interrupt_handlers.h"
#include "error_codes.h"
#include "interrupts.h"
#include "logging.h"
#include "rtc.h"
#include "string.h"
#include "usb_comm.h"

extern uint8_t setTime;

void usartMessageHandler(void)
{
    if (fullMessageReceived) // If a full message has been received, then process the received
                             // transmission information
    {
        if (setTime == 1)
        {
            // If the program progresses here, it should have received the time data

            int8_t returnCode = RTC_SetTime(); // Attempt to set the RTC with the received data
            if (returnCode)
            {                       // If an error occurs...
                switch (returnCode) // Log the appropriate error
                {
                    case 1:
                        logError(RTC_FORMATTING_ERROR, -1); // That could be a formatting error...
                        break;
                    case 2:
                        logError(RTC_INVALID_DATETIME_ERROR, -1); //...or maybe this error
                        break;
                    case 3:
                        logError(RTC_SET_ERROR, -1); //...or maybe even this error!
                        break;
                    default:
                        logError(UNRECOGNIZED_ERROR_RECEIVED, -1);
                        break;
                }
                // If an error occurred, resend the "SEND_TIME" message and keep setTime = 1
                usb_print("SEND_TIME\r\n");
                setTime = 1;
            }
            else // Otherwise reset setTime to 0
            {
                setTime = 0;
            }
        }
        else if (strcmp((const char *)usartMessage, "SET_TIME") == 0)
        { // If "SET_TIME" was received...
            // Set the boolean variable to set the time after the next full message is received...
            setTime = 1;
            //...and send a "SEND_TIME" message via USART
            usb_print("SEND_TIME\r\n");
        }
        else // If none of the above commands were received, log an error
        {
            logError(UNRECOGNIZED_COMMAND_RECEIVED, -1);
        }
        /*CAN ADD FUTURE USART COMMANDS TO BE PROCESSED TO THIS CHAINED IF-ELSE BLOCK*/

        // Since the current message should now have been fully processed, reset the
        // 'fullMessageReceived' flag to false.
        fullMessageReceived = 0;
    }
    return;
}