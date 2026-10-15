# Redundant Temperature Sensor System

A fault-tolerant temperature monitoring system built around an STM32L476RG microcontroller and three redundant TMP102 temperature sensors.

The system periodically reads the sensors, evaluates the readings for communication failures, out-of-range values, and disagreement between redundant measurements, and determines a final temperature value from the available valid readings.

Sensor status (and certain changes in sensor status) is communicated through LEDs and an active buzzer, while timestamped temperature and error information is transmitted to a host computer over USART.

---

## Overview

This project includes code for **a redundant temperature sensor system** to be run on an STM32L476RG microcontroller.

This system:

- Periodically samples multiple temperature sensors over a shared I²C bus, utilizing a timer interrupt.
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
- Unity (C unit testing)
- Jasmine and Node.js (board-connected USART tests)
- GitHub Actions CI

---

## System Architecture

> **NOTE:** While the firmware logic that analyzes the temperature readings is designed to accommodate a variable number of temperature sensors, the default settings and code of the software overall expect 3 temperature sensors. This setup is used in the diagrams and explanations that follow.
> Adapting the complete system to another sensor count requires corresponding hardware and software changes. The astute engineer may discern how to do so.

The firmware is intended to run on an STM32L476RG microcontroller. TMP102 digital temperature sensors are connected to the microcontroller via a shared I2C bus and are intermittently polled after a timer interrupt sets a flag to do so during the next pass through the main function's while loop.

The received sensor readings are then analyzed by the firmware, and a final 'display temperature' is determined. Based on both the readings from the sensors and the results of the determination logic running on the microcontroller, LED indicators associated with each sensor are then updated, and data is sent to a host computer via a USART connection.

The following diagram visualizes connections between different physical components of the system.

![A diagram showing the types of connections between different hardware components of the system, and purposes/jobs of different components in the system.](images/RTSHardwareArchDiagram.png "Hardware Architecture Diagram")

---

## Hardware

### Main Components

- STM32 Nucleo-L476RG
- 3 × TMP102 temperature sensors
- 2 × 74HC595 shift registers
- Active buzzer
- Red, yellow, and green LEDs
- 220 Ω resistors
- USB / micro-USB connection to host computer

### Sensor Configuration

The default firmware configuration expects three TMP102 sensors using the following I²C addresses:

| Sensor   | TMP102 Address |
| -------- | -------------: |
| Sensor 0 |         `0x48` |
| Sensor 1 |         `0x49` |
| Sensor 2 |         `0x4A` |

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
│   ├── interrupt_handlers.c
│   ├── interrupts.c
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
│   ├── interrupt_handlers.h
│   ├── interrupts.h
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
       Sensor 0  ─┐
       Sensor 1  ─┼── I²C ──► Sensor Readings
       Sensor 2  ─┘                │
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

If more than one reading is available for consideration, then the available sensor readings are compared against one another.

If a reading's value differs from the current average by at least the configured disagreement threshold, it is marked as an outlier:

```text
IS_OUTLIER
```

The average is then recalculated using the remaining valid reading values.

This process continues until the remaining reading values no longer disagree beyond the configured threshold.

If this process removes all but one reading, the last reading is marked as an outlier as well (every reading then disagrees with every other), an `ALL_READS_MARKED_INVALID` error is reported, and the display temperature is chosen as described below.

Agreement between sensors is trusted over the sensors' specified operating range. Therefore, when two sensors agree with each other on a value outside of the sensors' specified operating range, disagreeing with a third reading that is within the sensors' specified range, then they will be trusted over the third, outlier value (even though they are out of bounds and the third is within the specified operating temperature bounds).

In such a case, the two agreeing readings would still be flagged by the bounds check as being out of the specified sensor operating bounds, but their average would still become the display temperature.

> **NOTE:** Outlier detection runs before readings are checked for being out of specified operating bounds, and it runs only when more than one reading is available.

### 3. Temperature Bounds

After outlier processing, every available reading (including readings already marked as outliers) is checked against the configured upper and lower temperature bounds (by default set to the manufacturer's specified operating range for the sensors). A reading can therefore carry both an outlier flag and a bounds flag.

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

- Average the readings that carry no fault flags (the normal case).
- Use the only available reading when just one sensor is communicating. The bounds check still runs on it, so the reading is displayed even if it is flagged as out of range.
- Choose one reading to display when all readings are outliers of each other. Specifically the last reading marked as an outlier will be selected, if that reading is also within the specified sensor operating range. Otherwise, the last reading marked as an outlier that is also within the sensor operating range will be selected. However, if none of the readings are within the specified sensor operating bounds, then the last reading marked as an outlier will still be selected, in spite of also being out of the specified operating range.
- Average agreeing readings that are outside the specified sensor operating range, if all in-bounds readings are marked as outliers (e.g. the two-agreeing-sensors case described in [Outlier Detection](#2-outlier-detection) above). In such a case, an `ALL_READS_MARKED_INVALID` error is reported with no sensor number.
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

Messages can be sent either to or from the microcontroller. The only task currently intended to be done by sending data to the microcontroller is setting the RTC; see [Communication to the Microcontroller](#communication-to-the-microcontroller) later in this section.

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
D: timestamp, display temperature, sensor 0 temperature, sensor 1 temperature, sensor 2 temperature, sensor 0 faults, sensor 1 faults, sensor 2 faults
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
- Sensor Values - The temperature values (in degrees Celsius) derived from the readings of each temperature sensor, in sensor order (sensor 0, 1, 2), separated by commas. (If a reading was missing, the value is transmitted as "--.--").
- Sensor Faults - A string of symbols representing different 'faults' associated with each temperature sensor reading, in the same sensor order.

> **NOTE:** Sensor numbers in messages are 0-based: sensors 0, 1 and 2 are the sensors at `0x48`, `0x49` and `0x4A`, in that order (as in [Sensor Configuration](#sensor-configuration) above).

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
| 999        |   `UNRECOGNIZED_ERROR_RECEIVED` |

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

Testing is organized to distinguish **verification of individual software behaviors** from **validation of the integrated device**. Passing host-side tests and successful firmware builds do not, by themselves, establish that the assembled temperature-monitoring system behaves correctly under physical fault conditions.

### Test Strategy and Basis

| Level                         | Question answered                                                                                                             | Environment                                                                                                                                                         |
| ----------------------------- | ----------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Unit tests**                | Do individual firmware functions produce the expected results, including on error and boundary inputs?                        | Native C executables using [Unity](https://github.com/ThrowTheSwitch/Unity), with STM32 HAL fakes and logging/communication test doubles; no STM32 required.        |
| **USART system tests**        | Does the running firmware handle serial commands and recovery scenarios correctly through the actual board interface?         | [Jasmine](https://jasmine.github.io/) and [Node SerialPort](https://serialport.io/) communicating with a programmed Nucleo board over its ST-LINK virtual COM port. |
| **Physical/bench validation** | Do the assembled sensors, indicators, buzzer, and firmware work together under normal operation and injected hardware faults? | Real TMP102 sensors and connected hardware; manually performed and **not fully completed**.                                                                         |

Firmware compilation and deployment checks provide additional evidence that the application can be built and programmed, but are not substitutes for functional tests.

The **test basis** is the expected behavior documented in this README—particularly [Sensor Redundancy and Fault Detection](#sensor-redundancy-and-fault-detection), [Communication Protocol](#communication-protocol), and [Project Configuration](#project-configuration)—together with the implemented interfaces and hardware specifications. Where the documented behavior did not prescribe an edge-case outcome, tests also _characterize_ the implementation's chosen behavior, rather than asserting that the choice is universally correct. This project does not currently maintain a separate, controlled requirements specification or formal requirements-to-test traceability matrix.

### Results and Reproducibility

**Evidence current as of October 14, 2026.** Results below distinguish tests rerun while reviewing this source archive from hardware-related results reported by the project author.

| Activity                              | Latest known result                      | Evidence / scope                                                                                                                                              |
| ------------------------------------- | ---------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Unity application tests**           | **102 passed; 0 failed**                 | Rebuilt and executed on a Linux host with GCC, CMake, and Ninja on October 14, 2026. Includes determination logic, sensor reading, and command-handler tests. |
| **Unity RTC tests**                   | **51 passed; 0 failed**                  | Rebuilt and executed on the same Linux host on October 14, 2026.                                                                                              |
| **Jasmine USART tests**               | **17 specs previously reported passing** | Run by the author against a connected STM32. The physical-board suite was not rerun for this README review.                                                   |
| **Firmware Debug and Release builds** | **Previously reported passing**          | ARM cross-compilation locally and in GitHub Actions; not independently rerun during this review.                                                              |
| **Programming and debugging**         | **Previously reported passing**          | ST-LINK programming and launch of Release firmware; Debug session reaching `main()` with source-level debugging.                                              |
| **JavaScript syntax**                 | **Check passed locally**                 | `node --check tests/system/usart/spec/usart.spec.js` on October 14, 2026. This syntax check does not execute the board-connected Jasmine tests.               |

These numbers are **individual Unity test cases** or **Jasmine specs**, not a percentage of code coverage. CTest registers the two Unity executables as two test entries. No statement/branch coverage percentage or benchmark runtime is claimed here.

### Host-Side C Unit Tests

**Location:** [`tests/unit/`](tests/unit/) and [`tests/CMakeLists.txt`](tests/CMakeLists.txt). The host build compiles selected production C files against [`tests/fakes/`](tests/fakes/) and [`tests/mocks/`](tests/mocks/), allowing HAL responses, sensor input, and logging effects to be controlled without the microcontroller.

| Area                                | Behaviors exercised                                                                                                                                                                                                             |
| ----------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Redundant temperature decisions     | Averaging available valid sensors; each sensor missing or rejected; agreement/disagreement and outlier selection; all-outlier fallback behavior; upper/lower bounds, exact boundaries, and mixed in-/out-of-range measurements. |
| TMP102 communication and conversion | Positive/negative temperature decoding, sensor read failures, one retry after failed I²C transmit/receive, no unnecessary retry on success, and reported missing-read errors.                                                   |
| USART command handling              | Recognition of `SET_TIME`, rejection of an unrecognized command, ignoring an incomplete message, and successful date/time dispatch using a stubbed RTC setter.                                                                  |
| RTC setting logic                   | Date/time formatting and value validation, month/leap-year/century boundaries, weekday calculation, HAL set failures, and century state updates.                                                                                |

**Run from the repository root** after installing CMake, Ninja, and a native C compiler (for example, GCC on Linux or MSYS2 UCRT64 GCC on Windows):

```bash
cmake -S tests -B tests/build/Tests -G Ninja
cmake --build tests/build/Tests
ctest --test-dir tests/build/Tests --output-on-failure
```

For the individual Unity case names and totals, run the executables directly:

```bash
./tests/build/Tests/unit_tests
./tests/build/Tests/rtc_tests
```

On **Windows PowerShell**, use the `.exe` executables instead:

```powershell
.\tests\build\Tests\unit_tests.exe
.\tests\build\Tests\rtc_tests.exe
```

The fakes demonstrate application responses to simulated HAL results; they do **not** simulate I²C electrical behavior, the real sensor's accuracy, interrupt timing, or the complete STM32 hardware environment.

### Board-Connected USART System Tests

**Location:** [`tests/system/usart/spec/usart.spec.js`](tests/system/usart/spec/usart.spec.js). The 17 Jasmine specs exercise the actual firmware through USART2 over the Nucleo board's ST-LINK virtual serial port at **115200 baud**. They cover LF/CRLF termination; accepted commands and RTC input; rejection and correction of malformed input; consecutive/rapid commands; preservation of a pending message; receive-buffer boundary and overflow behavior; and eventual recovery after an overflow.

For overflow, the implementation **does not discard every remaining byte until the next newline**. Remaining bytes may form a separate message, and an immediately following command may be rejected. The recovery checks establish that a _later_ valid command can be processed; they do not require the first command after an overflow to succeed.

**Run with a programmed, powered Nucleo board connected** and with any other serial monitor closed. Install Node.js and npm, then from the repository root:

```bash
cd tests/system/usart
npm ci
npm test
```

The test suite uses **`COM3` by default** or the serial port specified by the `RTS_PORT` environment variable. For example, before `npm test`:

```powershell
# Windows PowerShell
$env:RTS_PORT = "COM3"
```

```bash
# Linux (use the device path actually assigned to your board)
export RTS_PORT=/dev/ttyACM0
```

Each spec performs a `SET_TIME` exchange to establish a known starting state. Tests therefore **change the device's RTC** and wait for subsequent data messages. An active serial connection and correctly running firmware are required. These tests do not verify the physical temperature sensors, shift registers/LEDs, or buzzer.

### GitHub Actions Continuous Integration

The workflow is maintained in [`.github/workflows/ci.yml`](.github/workflows/ci.yml) and is triggered on pushes, pull requests, or manual dispatch. The workflow includes three categories of checks: native C unit tests, ARM firmware builds, and JavaScript syntax checks.

| CI job              | Automated check                                                                                                      | What it does **not** establish                                |
| ------------------- | -------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------- |
| `unity-tests`       | Configures and builds both native C test executables; executes them with CTest.                                      | Real sensor, peripheral, or hardware timing behavior.         |
| `firmware-build`    | ARM cross-compiles both `Debug` and `Release` firmware using the project's CMake presets.                            | Successful flashing, boot, or operation on the board.         |
| `javascript-checks` | Uses Node.js 24, runs `npm ci` in `tests/system/usart`, and syntax-checks `spec/usart.spec.js` using `node --check`. | Passing Jasmine specs or a working USART/physical connection. |

**The hardware-connected Jasmine suite does not run in GitHub-hosted CI.** Consequently, a green CI result means the configured source, build, and host-unit checks passed—not that the complete assembled temperature-monitoring system was validated.

### Physical Validation and Remaining Gaps

The automated suites and previously reported board-connected USART results should be complemented by **bench tests of the assembled device**. Physical results for the following areas are not documented as complete in this source archive:

- Normal operation with all three TMP102 sensors and operation with one, two, or zero communicating sensors, including disconnect/reconnect recovery.
- Real-sensor disagreement and temperature-boundary fault scenarios, including the resulting display-temperature choice.
- Per-sensor LED indications and buzzer behavior during fault transitions and all-sensors-unavailable conditions.
- Outgoing `D:` and `E:` records, including timestamps, temperatures, fault symbols, and ordering under actual hardware faults.
- Sampling cadence, non-hanging behavior when I²C communication fails, repeated fault/recovery cycles, and longer-duration operation.

No environmental qualification, calibrated temperature-accuracy measurement, or formal worst-case timing analysis is claimed. In particular, the unit-level all-outlier fallback is a **documented design choice**, not proof that a chosen temperature remains reliable when all readings have been marked invalid. Likewise, the serial overflow tests establish the implemented recovery behavior rather than guaranteeing lossless command delivery.

### Test Traceability and Maintenance

The behavior-to-suite tables above provide **informal traceability** to the behavioral descriptions in this README and to the linked test directories. Test names describe scenarios and expected outcomes; there are no formal requirement IDs. No verified defect-discovery history is maintained here, so this README does not attribute specific fixed defects to particular tests.

To extend the tests:

1. Add a C test under [`tests/unit/`](tests/unit/); register its source in [`tests/CMakeLists.txt`](tests/CMakeLists.txt) and its `RUN_TEST(...)` call in [`tests/unit/test_runner.c`](tests/unit/test_runner.c). RTC tests use their own runner in [`tests/unit/rtc/test_rtc.c`](tests/unit/rtc/test_rtc.c).
2. Add a serial spec named `*.spec.js` under [`tests/system/usart/spec/`](tests/system/usart/spec/) so it is discovered by [`jasmine.mjs`](tests/system/usart/spec/support/jasmine.mjs). Extend the JavaScript syntax-check CI step to include new spec files if applicable.
3. Name tests for the **behavior and condition being checked**, rerun the affected suite, and update this section when counts, coverage, or known limitations change.

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

Additionally, although the logic checking whether a date-time value provided for setting the RTC is valid, including considering leap years, the microcontroller itself may consider any year that ends in 00 as a leap-year. Operation across century boundaries has not been physically validated; a time correction may be required.

> **NOTE:** In short, running this code around the turn of a century may require a date-time correction on New Year's Day, as well as on March 1st.

### Validation scope

The automated results above may not fully establish fitness for safety-critical, production, or unattended monitoring use. There may also be bugs or oversight in either the test code, the firmware code, or both.

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
