# Redundant Temperature Sensor System

A fault-tolerant temperature monitoring system built around an STM32-L476RG microcontroller and three redundant TMP102 temperature sensors.

The system periodically reads the sensors, evaluates the readings for communication failures, out-of-range values, and disagreement between redundant measurements, and determines a final temperature value from the available valid readings.

Sensor status (and certain changes in sensor status) is communicated through LEDs and an active buzzer, while timestamped temperature and error information is transmitted to a host computer over USART.

---

## Overview

This project includes code for **a redundant temperature sensor system** to be run on an STM32-L476RG microcontroller.

This system:

- Periodically samples multiple temperature sensors over a shared I²C bus, utilizing a timer interrupt,
- Detects sensor communication failures.
- Detects temperature measurements outside the sensor's specified operating range.
- Identifies measurements that deviate significantly from the other measurements received.
- Uses the results from the above analyses to determine a final display temperature.
- Drives status LEDs through a pair of 74HC595 shift registers.
- Activates an audible buzzer when sensor measurements are first flagged or go missing.
- Maintains a real-time clock (RTC) for timestamping measurements and errors.
- Communicates timestamped measurements and fault information to a host computer over USART.

The project serves as a demonstration of embedded C programming, peripheral communication, and redundant-sensor decision logic.

---

## Topics/Technologies Involved

- Embedded C
- STM32L4 microcontrollers
- STM32 HAL
- TMP102 temperature sensors
- I²C communication
- SPI communication
- USART communication
- RTC timestamping
- Timer interrupts
- USART receive interrupts
- Fault detection
- Redundant sensor processing
- CMake
- Ninja
- ARM GNU Toolchain
- Visual Studio Code
- Cortex-Debug
- ST-LINK

---

## System Architecture

> **NOTE:** While the firmware logic that analyzes the temperature readings is designed to accommodate a variable number of temperature sensors, the default settings and code of the software overall expect 3 temperature sensors. This setup is used in the diagrams and explanations that follow.
> The astute engineer may discern how to modify the hardware (and what software constants and lines of code may need to be changed) to use the software with other numbers of sensors.

The firmware is intended to run on an STM32-L476RG microcontroller. TMP102 digital temperature sensors are connected to the microcontroller via a shared I2C bus and are intermittently polled after a timer interrupt sets a flag to do so during the next pass through the main function's while loop.

The received sensor readings are then analyzed by the firmware, and a final 'display temperature' is determined. Based on both the readings from the sensors and the results of the determination logic running on the microcontroller, LED indicators associated with each sensor are then updated, and data is sent to a host computer via a USART connection.

The following diagram visualizes connections between different physical components of the system.

![A diagram showing the types of connections between different hardware components of the system, and purposes/jobs of different components in the system.](images/RTSHardwareArchDiagram.png "Hardware Architecture Diagram")

---

## Hardware

### Main Components

- STM32 Nucleo-L476RG
- 3 × TMP102 temperature sensors
- 74HC595 shift register(s)
- Active buzzer
- Red, yellow, and green LEDs
- 220 Ω resistors
- USB / micro-USB connection to host computer

### Sensor Configuration

The default firmware configuration expects three TMP102 sensors using the following I²C addresses:

| Sensor   | TMP102 Address |
| -------- | -------------: |
| Sensor 1 |         `0x48` |
| Sensor 2 |         `0x49` |
| Sensor 3 |         `0x4A` |

The sensors share the same I²C bus.

For more information on TMP102 temperature sensors (including how to assign the above addresses to different sensors) feel free to explore the datasheet available on [this page](https://www.ti.com/product/TMP102) of Texas Instruments' website.

### Status LEDs

Each sensor has a red, yellow, and green status LED.

The current implementation uses the LEDs to indicate the sensor's state:

| Sensor condition                        | LED indication |
| --------------------------------------- | -------------- |
| Valid reading                           | Green          |
| Questionable / flagged reading          | Yellow         |
| Communication failure / missing reading | Red            |

The LEDs are controlled through daisy-chained 74HC595 shift registers using SPI.

### Buzzer

An active buzzer is used to provide an audible indication when certain sensor condition changes occur.

A short beep is generated when a sensor becomes newly faulty (experiences a communication failure, or has its reading flagged as being an outlier or out of the sensor's specified operating range). If all sensors become unavailable, the buzzer enters an extended/continuous alert state.

---

## Software Architecture

The firmware is divided into .c files inside of `Core/App/`, with the main.c file (initially generated by CubeMX, but edited by the author of this project) under `Core/Src/`, along with other STM generated or included .c files.

```text
Core/
├── App/
│   ├── alerts.c
│   ├── determination_logic.c
│   ├── hardware.c
│   ├── interrupts.c
│   ├── interrupt_handlers.c
│   ├── logging.c
│   ├── rtc.c
│   ├── sensor.c
│   └── usb_comm.c
│
├── Inc/
│   ├── alerts.h
│   ├── defines.h
│   ├── determination_logic.h
│   ├── error_codes.h
│   ├── faults.h
│   ├── hardware.h
│   ├── interrupts.h
│   ├── interrupt_handlers.h
│   ├── logging.h
│   ├── rtc.h
│   ├── sensor.h
│   ├── usb_comm.h
│   └── STM32/CubeMX-generated / included header files
│
└── Src/
    ├── main.c
    └── STM32/CubeMX-generated source
```

### High-Level Data Flow

```text
       TMP102 Sensors
       Sensor 1  ─┐
       Sensor 2  ─┼── I²C ──► Sensor Readings
       Sensor 3  ─┘                │
                                   ▼
                   Fault Detection & Redundancy Logic
                                   │
                    ┌──────────────┼──────────────┐
                    ▼              ▼              ▼
              Display Temp       LEDs          Buzzer
                    │
                    ▼
             Timestamp + Data
                    │
                    ▼
              USART / USB
                    │
                    ▼
             Host Computer
```

### Main-Loop / Interrupt Architecture

The timer and USART peripherals use interrupts, but substantial processing is intentionally performed outside the interrupt service routines.

The general execution model is:

```text
Sensor Polling Timer Interrupt
      │
      ▼
Set readNow flag
      │
      ▼
Main Loop
      │
      ├── Read sensors
      ├── Flag questionable / missing readings
      ├── Determine display temperature
      ├── Log data
      ├── Update LEDs / buzzer
      └── Prep for next sensor read
```

The main loop is also set to, on each iteration through the loop, check and handle flags set or errors logged during execution of interrupt code, as well as to check the state of the buzzer (a non-blocking way to turn off the buzzer after a certain amount of time has elapsed).

USART reception follows a similar pattern:

```text
USART Receive Interrupt
      │
      ▼
Receive bytes into buffer
      │
      └── If message terminator received or error occurs
      │                       │
      │                       ▼
      │         Set flags/errors based on message received or error that occurred
      │
      ▼
Main Loop
      │
      └── Check for and/or handle flags set or errors logged
```

This keeps relatively time-consuming operations such as sensor communication, logging, and received RTC data processing out of interrupt callbacks.

---

## Sensor Redundancy and Fault Detection

Each sensor is represented by a `Sensor` structure containing its I²C address, current (most recent) temperature reading value, fault flags, the previous reading's fault flags, and data associated with the LEDs related to that sensor's measurements.

The firmware evaluates sensor readings in several stages.

### 1. Communication Check

If an I²C communication fails, the sensor is marked with a communication fault:

```text
COMM_FAULT
```

Its current temperature is treated as unavailable.

### 2. Outlier Detection

If more than one reading is available for consideration, then existing sensor readings are compared against one another.

If a reading's value differs from the current average by at least the configured disagreement threshold, it is marked as an outlier:

```text
IS_OUTLIER
```

The average is then recalculated using the remaining valid reading values.

This process continues until the remaining reading values no longer disagree beyond the configured threshold.

### 3. Temperature Bounds

After outlier processing, temperature reading values are checked against the configured upper and lower temperature bounds (by default set to the manufacturer's specified operating range for the sensors).

Measurements outside those limits are marked with:

```text
ABOVE_BOUNDS
```

and

```text
BELOW_BOUNDS
```

respectively. Note that, since outlier processing only occurs if more than one reading value is available, yet even a single reading can be above or below the specified sensor operating range, this bounds-checking process occurs even if there is only one reading value available (and no outlier detection has occurred).

### 4. Final Temperature

In light of the results of the aforementioned analyses and the sensor readings available, a final display temperature is determined.

The firmware may:

- Average multiple valid sensors.
- Use the only remaining sensor reading value (if only one sensor reading remains unflagged).
- Choose a reading from one sensor to display (when all readings are outliers of each other).
- Average multiple flagged readings to display (when all readings have been flagged as missing or as potentially invalid or questionable).
- Return `NAN` when no sensor readings are available.

The exact determination logic is implemented in `determination_logic.c`.

---

## Fault Flags

Sensor faults are represented as a bitmask, allowing multiple fault conditions to be associated with the same sensor.

| Flag           | Meaning                                                           |
| -------------- | ----------------------------------------------------------------- |
| `COMM_FAULT`   | Communication with the sensor failed                              |
| `ABOVE_BOUNDS` | Temperature reading above the sensor's specified upper bound      |
| `BELOW_BOUNDS` | Temperature reading below the sensor's specified lower bound      |
| `IS_OUTLIER`   | Reading was rejected because it disagreed with the other readings |

Because these are bit flags, multiple faults can be simultaneously associated with one sensor (e.g. a reading can be both above bounds and an outlier).

---

## Communication Protocol

The program is set to communicate with a host computer via USART (USB) using a baud rate of 115200. The STM32's USART connection is exposed to the host computer through the Nucleo board's USB connection.

Messages can be sent either to or from the microcontroller (although the only task currently intended to be done through sending data to the microcontroller is setting the RTC (see (Communication to the Microcontroller)[#communication-to-the-microcontroller] later in this section)).

### Communication From the Microcontroller

Messages sent from the microcontroller to a host computer via USART use the following general structure:

```text
<header><timestamp><message body>\r\n
```

The **header** consists of a letter identifying the type of message being transmitted, a colon, and a space. There are two different headers corresponding to the different message types:

- Data (`D: `)
- Error (`E: `)

#### Data Messages

Data messages begin with:

```text
D:
```

The message contains:

```text
D: timestamp, display temperature, sensor 1 temperature, sensor 2 temperature, sensor 3 temperature, sensor 1 faults, sensor 2 faults, sensor 3 faults
```

Example:

```text
D: 09/10/2026 10:42:18, 24.75, 24.75, 24.69, 24.81, , ,
```

A missing temperature is represented by:

```text
--.--
```

A description of the different pieces of information follows:

- Timestamp - The timestamp for the specific batch of data.
- Display Temperature - The temperature returned by the microcontroller logic after analyzing the available individual temperature sensor read values.
- Sensor Values - The temperature values (in degrees Celsius) derived from the readings of each temperature sensor, separated by commas. (If a reading was missing, the value is transmitted as "--.--").
- Sensor Faults - A string of symbols representing different 'faults' associated with each temperature sensor reading.

#### Fault Symbols

Sensor fault strings use the following symbols:

| Symbol  | Meaning                               |
| ------- | ------------------------------------- |
| `` ` `` | Communication / missing-reading fault |
| `*`     | Outlier                               |
| `-`     | Below configured temperature range    |
| `+`     | Above configured temperature range    |

Multiple symbols can appear together when a sensor has multiple fault flags. Or, a string of fault flags can simply be an empty string if no flags have been marked for that sensor reading.

#### Error Messages

Error messages begin with the error message header:

```text
E:
```

and include a timestamp followed by the error code (a unique three digit number enclosed in square brackets []) and a message describing the error or situation that occurred that led to the sending of the message. Some examples include:

```text
E: 09/10/2026 10:42:18 [201] Sensor 1 reading missing.
```

```text
E: 09/10/2026 10:42:20 [303] Sensor 2 reading marked invalid as an outlier.
```

```text
E: 09/10/2026 10:42:22 [200] All sensor readings missing.
```

They are also terminated by carriage return and newline characters. A table of different errors and their error codes is shown below.

| Error Code |                           Error |
| ---------- | ------------------------------: |
| 100        |          `SENSORS_NOT_DETECTED` |
| 200        |      `ALL_SENSOR_READS_MISSING` |
| 201        |           `SENSOR_READ_MISSING` |
| 300        |      `ALL_READS_MARKED_INVALID` |
| 301        |             `READ_ABOVE_BOUNDS` |
| 302        |             `READ_BELOW_BOUNDS` |
| 303        |        `READ_MARKED_AS_OUTLIER` |
| 400        |               `NOT_ENOUGH_LEDS` |
| 500        | `UNRECOGNIZED_COMMAND_RECEIVED` |
| 501        |          `RTC_FORMATTING_ERROR` |
| 502        |    `RTC_INVALID_DATETIME_ERROR` |
| 503        |                 `RTC_SET_ERROR` |
| 504        |                `RX_BUFFER_FULL` |
| 505        |            `RX_BUFFER_OVERFLOW` |
| 000        |   `UNRECOGNIZED_ERROR_RECEIVED` |

### Communication to the Microcontroller

#### Receive Buffer Behavior

The firmware maintains a single pending received message. If a complete message has not yet been processed by the time another message is fully received, the additional message is discarded and an `RX_BUFFER_FULL` error is reported, along with a request for the message to be resent.

Messages exceeding the receive buffer size generate an `RX_BUFFER_OVERFLOW` error. Once the maximum number of characters have been received (by default 100 characters) without having received the terminating `\n`, the error is generated, the buffer is reset, and any remaining bytes are then processed as being part of a new message.

Messages are terminated by a LF (`\n`), which is included in the count towards the 100 max character limit for messages. CR (`\r`) characters are ignored (and do not count towards the 100 max character limit for messages), allowing either LF (`\n`) or CRLF (`\r\n`) line endings to terminate a message.

Messages that are not recognized as a valid command generate an `UNRECOGNIZED_COMMAND_RECEIVED` error, unless the system is in a state expecting to receive date/time data via USART, in which case different errors are logged if the data is considered invalid or incorrectly formatted in light of what the system is expecting.

The only command the firmware has currently been set up to recognize and handle via USART is setting the microcontroller's RTC, the process of which is explained below.

#### Initial Setting of the RTC Via USART Communication

The microcontroller will send a "`SEND_TIME\r\n`" message before entering normal operation. After this message is sent, the next received message is interpreted as the date/time value. The expected format is:

```text
YYYY/mm/dd HH:MM:SS
```

using 24-hour time.

Example:

```text
2026/09/10 10:45:30
```

This complete date and time data should be terminated by a newline character.

After receiving the date/time value, the firmware uses it to configure the STM32 RTC. If an error occurs during this process, the firmware reports an error and requests the time again. See the [RTC Errors](#rtc-errors) section below.

#### Resetting the RTC

The time may still be set after the first exchange of data if necessary.

If the microcontroller receives the command:

```text
SET_TIME\r\n
```

then the microcontroller will once again send a "`SEND_TIME\r\n`" message, and the next data received will be treated as the date, in the same format as described above. Once that date is received, that data will automatically be used to set the RTC. Again, if the supplied value is incorrectly formatted, the firmware will report an RTC formatting error and request the time again.

#### RTC Errors

The following are errors that may occur while setting the RTC:

| Error                        | Description                                                                                         |
| ---------------------------- | --------------------------------------------------------------------------------------------------- |
| `RTC_FORMATTING_ERROR`       | Expected data to set the RTC, but the received data was incorrectly formatted to do so              |
| `RTC_INVALID_DATETIME_ERROR` | An invalid date was received as input to set the RTC (e.g. February 30th)                           |
| `RTC_SET_ERROR`              | An error occurred while attempting to set the RTC using the valid, correctly formatted time or date |

---

## Build Instructions

### Prerequisites

To use the following build method, the following software must be installed:

- Visual Studio Code
- CMake
- Ninja
- ARM GNU Toolchain (`arm-none-eabi-gcc`)
- Git

The **ARM GNU Toolchain must be available on the system `PATH`**, as the project's CMake toolchain file expects to locate the ARM compiler using the `arm-none-eabi-` command prefix.

CMake and Ninja must also be accessible to the CMake/VS Code build environment.

You can verify the required command-line tools with:

```bash
cmake --version
ninja --version
arm-none-eabi-gcc --version
git --version
```

### Clone the Repository

Clone the repository and navigate to the project directory:

```bash
git clone https://github.com/CodeyBrody/RedundantTempSensor.git
cd RedundantTempSensor
```

You can [Build with VS Code](#build-with-vs-code) or [Build from the Command Line](#build-from-the-command-line). Both approaches are outlined below.

### Build with VS Code

The project uses the **CMake Tools** VSCode Extension with separate Debug and Release presets.

1. Open the project directory in Visual Studio Code.
2. Open the Command Palette with `Ctrl + Shift + P`.
3. Select **CMake: Select Configure Preset** and choose either `Debug` or `Release`.
4. Run **CMake: Configure**.
5. Run **CMake: Build**.

### Build from the Command Line

The project provides CMake presets for both Debug and Release builds.

#### Debug

```bash
cmake --preset Debug
cmake --build --preset Debug
```

#### Release

```bash
cmake --preset Release
cmake --build --preset Release
```

The CMake presets automatically configure the project to use the Ninja build system and the ARM GNU toolchain.

### Build Output

Both Debug and Release builds produce an **ELF executable**.

> **Note:** The Debug build includes debugging information and is configured for development and debugging. The Release build is optimized for size and does not include debugging information.

Generated build files are stored under:

```text
build/
├── Debug/
└── Release/
```

The generated build directory is excluded from version control.

## Flashing and Debugging

The project uses the **ST-LINK** programmer/debugger integrated into the **STM32 Nucleo-L476RG** board and the **Cortex-Debug** VS Code extension.

### Prerequisites

- Connect the Nucleo-L476RG to the computer via USB.
- Install the **Cortex-Debug** VS Code extension.
- Build either the **Debug** or **Release** configuration.

### Debug Build

1. Open the **Run and Debug** view in Visual Studio Code.
2. Select **Debug RedundantTempSensor**.
3. Press **F5** or select **Start Debugging**.

Cortex-Debug will program the Debug ELF through ST-LINK and start a debugging session.

### Release Build

1. Build the **Release** configuration.
2. Open the **Run and Debug** view.
3. Select **Release RedundantTempSensor**.
4. Press **F5** or select **Start Debugging**.

Cortex-Debug will program the Release ELF through ST-LINK and start the firmware.

---

## Verification and Validation

The software and test harness have undergone unit, serial-protocol and build/deployment verification. **A passing automated suite is not evidence that every assembled-hardware scenario has been verified.** Physical TMP102 breakout-board connections have not yet been reliable enough to complete the planned hardware-integration checklist. Some previous hardware-independent validation used a connected STM32 for USART testing.

### Completed verification

The results distinguish **independent host-side verification of the cleaned source** from **board-connected validation previously reported by the project author**. The two newly added determination-logic cases in the source archive needed their source file included in CMake and one expectation corrected to match the documented all-outlier fallback; these adjustments are included in this cleaned distribution:

| Area                                             | Result                                              | What was verified                                                                                                                                                                                                            |
| ------------------------------------------------ | --------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| C Unity — determination logic and sensor reading | **102 tests passed, 0 failures**                    | Sensor validity, averaging and outlier selection, upper/lower bounds, missing readings, signed TMP102 conversion, I²C failure/retry behavior, mocked sensor error logging, and two additional determination-logic edge cases |
| C Unity — RTC                                    | **51 tests passed, 0 failures**                     | Date/time syntax and valid ranges, month and leap-century boundaries, weekday calculation including year 0000, HAL failure handling, and century-update behavior                                                             |
| Jasmine — USART protocol                         | **17 specs passed (previously reported by author)** | Valid commands, LF/CRLF, malformed and invalid input, rapid messages, pending-message and buffer-overflow cases, and subsequent command recovery over the ST-LINK virtual COM port                                           |
| Firmware builds                                  | **Passed (previously reported by author)**          | Clean Debug and Release CMake/Ninja builds, with compiler warning resolved                                                                                                                                                   |
| Deployment and debugging                         | **Passed (previously reported by author)**          | Release firmware launched through ST-LINK; Debug ELF reached `main()` with source symbols and a functioning breakpoint                                                                                                       |

The **102 main Unity tests and 51 RTC tests were run successfully from the cleaned source on a Linux host using GCC**. The 17 Jasmine specs are **automated, board-connected USART tests**, not stand-alone host-only unit tests; they were previously reported passing on the STM32, but were not rerun as part of this offline cleanup. They verify the serial interface but do not substitute for testing the connected temperature sensors, indicators, and buzzer.

### Reproducing the automated tests

For the host-side C tests, install CMake, Ninja, and a native C compiler (such as MSYS2 UCRT64 GCC on Windows). From the repository root:

```bash
cmake -S tests -B tests/build/Tests -G Ninja
cmake --build tests/build/Tests --target unit_tests rtc_tests
```

Then execute both test programs. On Windows PowerShell:

```powershell
.\tests\build\Tests\unit_tests.exe
.\tests\build\Tests\rtc_tests.exe
```

On Linux, use `./tests/build/Tests/unit_tests` and `./tests/build/Tests/rtc_tests`. You may need to supply `-DCMAKE_C_COMPILER=...` during configuration if your native compiler is not on `PATH` or if a previously selected toolchain is cached.

For the hardware-connected Jasmine tests, connect the Nucleo board to your computer and ensure its firmware is running. Check the COM-port setting in `tests/system/usart/spec/usart.spec.js` (the supplied configuration uses `COM3`), and close any other serial-port monitor. Then run:

```bash
cd tests/system/usart
npm ci
npm test
```

Node.js and npm are required. The Jasmine tests send USART commands to the actual board; they cannot be run without it.

### Remaining physical/hardware validation

**Not yet completed:** the three-sensor baseline; one/two/zero sensor configurations; physical disconnect/reconnect and recovery; real-sensor negative/disagreement cases; RGB LED and buzzer responses; sampling interval, RTC progression/rollovers, and complete outgoing `D:`/`E:` log consistency; non-hanging behavior during faulty I²C communication. Recommended additional checks include repeated fault cycles, cold power-up, longer-duration stability, and comparison of Debug versus Release behavior.

These are outstanding **validation tasks**, not claimed test passes or defects. A concise checklist is available in [VALIDATION_STATUS.md](VALIDATION_STATUS.md).

---

## Project Configuration

Several application-level constants are defined in:

```text
Core/Inc/defines.h
```

For example:

```c
#define REQUESTED_SENSOR_SAMPLING_INTERVAL_SEC 2
#define DISAGREE_THRESHOLD 2.0
#define BUFFER_SIZE 100
```

`REQUESTED_SENSOR_SAMPLING_INTERVAL_SEC` controls the desired interval between sensor-reading cycles.

The default application configuration expects:

```text
3 sensors
```

and three sets of red, green and yellow status LEDs.

---

## Host Computer Software

This software was originally intended to work in tandem with logging/display software on the host computer connected via USART to the microcontroller. While messages sent via USART can be viewed in a general serial monitor, this program relies on receiving data from the host computer (either sent by program running on the computer or receiving data sent by a human) to configure the RTC.

To view one such program, feel free to check out the repository [here](https://github.com/CodeyBrody/TMP102RTSProjectLDSoftware.git).

If you would like to create your own, feel free to take a look at the [Communication Protocol](#communication-protocol) section above to see how data communicated over USART is structured.

---

## Known Limitations

### Fixed Hardware Configuration

Although portions of the sensor-processing logic operate on a sensor array, the current overall code is configured specifically for three TMP102 sensors.

Changing the number of sensors may require corresponding changes to the hardware and code.

### Timing

Sensor sampling is initiated by a timer interrupt and processed by the main loop. The configured interval represents the desired, approximate interval between reading cycles rather than a guarantee that every cycle will begin at an exact wall-clock time.

### RTC century handling

USART date/time input uses a four-digit year. The STM32 RTC calendar stores only the last two digits; the application retains the century separately in RAM for timestamp formatting and updates it after a successful set operation. Century information is **not persistent across reset or power loss**, and automatic `99` to `00` year rollover is not tracked (i.e. the century is not automatically updated if the firmware is running during the turn of a century).

Additionally, while the the logic checking whether a date-time value provided for setting the RTC is valid, including considering leap years, it seems the microcontroller itself considers any year that ends in 00 as a leap-year. (So, in short, running this code around the turn of a century may require date-time correction on New Year's Day, as well as March 1st ;P ).

### Validation scope

The automated results above may not fully establish fitness for safety-critical, production, or unattended monitoring use. Their may also be bugs or oversight in either the test code, the firmware code, or both.

Currently no warranties are given for fitness for any particular purpose, and the code is not automatically approved for just any use by anyone. However, anyone who does use the firmware, or relies upon any information or code related to this repository or project, does so "as-is" and at their own risk. Contact [@CodeyBrody](https://github.com/CodeyBrody) with questions.

---

## Future Improvements

Potential future development opportunities include:

- Complete the outstanding physical integration and longer-duration validation checks, as well as additional unit, system, and/or integration tests
- Making the overall hardware/firmware easier to adapt to different sensor counts.
- Additional USART commands.
- Factor additional factors that could indicate questionable readings (e.g. a sudden, large increase or decrease in a sensor reading's value) into determination logic.

---

## License

This repository contains original application code as well as software components generated or provided by STMicroelectronics and other third parties.

Original code is provided by the author. Vendor-provided and third-party components retain their respective copyrights and license terms.

See the applicable license files and copyright notices included with those components.

At this time, no single license is intended to supersede the individual licenses applicable to vendor-provided or third-party components.

> NOTE: Parts of this README.md file were created using ChatGPT, mixed with content from and edited by @CodeyBrody. Parts of this README.md may be incorrect or incomplete, or may become outdated. If you discover any such issues, feel free to open a pull request, or contact the owner of this repository. (Who, although writing in the third person, did indeed write this note himself. 😉)
