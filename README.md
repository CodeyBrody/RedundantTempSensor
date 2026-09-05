# Redundant Temperature Sensor System

## Overview

This project includes code for a redundant temperature sensor system to be run on an STM32L476RG microcontroller.

Practically, this system:
- reads multiple temperature sensors (3 by default) at regular intervals,
- determines which temperature sensor measurement readings are to be considered invalid based on factors such as:
  - whether a measurement deviates significantly from the other measurements received, or
  - whether a measurement is outside of (above or below) the temperature sensor manufacture's specified operating range, and
- based on that determination, determines a final "display temperature" value, which is sent (along with other information) to a
  computer connected to the microcontroller.
  
The program includes code to:
- Set LEDs associated with each sensor to indicate the status of the temperature reading associated with that sensor (e.g. reading marked Valid, Invalid, or Missing)
- Operate a buzzer to sound when a sensor's readings transition to being marked as invalid or sensor readings become unavailable from a particular sensor.

## Features

- Multiple (redundant) TMP102 temperature sensors
- I2C communication
- SPI communication
- UART communication
- RTC timestamping
- Non-blocking timing
- Timer interrupt handling
- USART interrupt handling

## Hardware

- STM32 Nucleo-L476RG
- TMP102 Temperature Sensors
- 74HC595 IC Shift Registers
- USB -> micro-USB cable
- Active Buzzer
- 220Ω Resistors
- LEDs
  
## Software

- C
- STM32 HAL
- ARM GNU Toolchain
- CMake
- Ninja
- VS Code

## Architecture

The physical architecture of this project may be viewed from the perspective of the different connections between the microcontroller and the other components of the system. 

## Communication Protocol

The program is set to communicate with a host computer via USART (USB) using a baud rate of 115200.

Messages can be sent either to or from the microcontroller, although the only task intended to be done through sending data to the microcontroller is setting the RTC (see later in this section).

### Communication From the Microcontroller

Messages sent from the microcontroller to a host computer connected via USART are structured with a header, a message body, and are terminated with a 
carriage return and newline (`\r\n`). 

The **header** consists of a letter identifying the type of message being transmitted, a colon, and a space. There are two different headers corresponding to the different message types: 
- Data (`D: `)
- Error (`E: `)

#### Data Messages

The message bodies of data messages include the following information, separated by commas:
- Timestamp - The timestamp for that specific batch of data
- Display Temperature - The temperature returned by the microcontroller logic after analyzing the individual temperature sensor read values
- Sensor Values - The temperature values (in degrees Celsius) derived from the readings of each temperature sensors, separated by commas. (If a reading was missing,
  the value is transmitted as "--.--")
- Sensor Faults - A string of symbols representing different 'faults' associated with each temperature sensor reading, with each string of faults (one for each sensor) separated by a comma, translated as follows:
  - ' -> Reading missing
  - \* -> Reading marked as an outlier.
  - ^ -> Reading above specified sensor operating range
  - v -> Reading below specified sensor operating range
The message body is not terminated by a comma, but by a carriage return and newline.

#### Error Messages

Error messages begin with the error message header, followed by a message describing the error or situation that occurred that led to the sending of the message. They are also terminated by carriage return and newline characters.

### Communication to the Microcontroller

Communicating information via USART to the microcontroller triggers an interrupt, which receives data from the USART connection until a `\n` or `\r` is encountered, or until one less byte than the set size of the receiving buffer (by default 100 characters) is received (whichever comes first). 

The only current task the firmware has been set up to handle via being communicated to by USB is setting the RTC, the process of which is explained below.

#### Setting the RTC Via USART Communication

If a `\n` of `\r` is encountered, the message is compared to the string "`SET_TIME`". If the string does not match that message, the buffer is reset. Otherwise (if `SET_TIME` us received), the microcontroller sends the message "`SEND_TIME\r\n`", and the next data received will be treated as the date, in the format "`YYYY/mm/dd HH:MM:SS`", using the 24 hour format. This complete date and time data should be terminated by a carriage return and/or a newline character.

## Build Instructions

### Prerequisites

The following software must be installed:

* Visual Studio Code
* CMake
* Ninja
* ARM GNU Toolchain (`arm-none-eabi-gcc`)
* Git

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

Open the project directory in Visual Studio Code.

### Build with VS Code

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

The Debug build includes debugging information and is configured for development and debugging. The Release build is optimized for size and does not include debugging information.

Generated build files are stored under:

```text
build/
├── Debug/
└── Release/
```

The `build/` directory contains generated files and is excluded from version control.

## Flashing and Debugging

The project uses the **ST-LINK** programmer/debugger integrated into the **STM32 Nucleo-L476RG** board and the **Cortex-Debug** VS Code extension.

### Prerequisites

* Connect the Nucleo-L476RG to the computer via USB.
* Install the **Cortex-Debug** VS Code extension.
* Build either the **Debug** or **Release** configuration.

### Debug Build

1. Open the **Run and Debug** view in Visual Studio Code.
2. Select **Debug RedundantTempSensor**.
3. Press **F5** or select **Start Debugging**.

Cortex-Debug will program the Debug ELF through ST-LINK and start a debugging session.

```text
build/Debug/RedundantTempSensor.elf
```

### Release Build

1. Build the **Release** configuration.
2. Open the **Run and Debug** view.
3. Select **Release RedundantTempSensor**.
4. Press **F5** or select **Start Debugging**.

Cortex-Debug will program the Release ELF through ST-LINK and start the firmware.

```text
build/Release/RedundantTempSensor.elf
```

> **Note:** The Release build is optimized for size and does not include debugging information, so source-level debugging is limited compared with the Debug build.

## Known Limitations

- While the logic for determining a 'display temperature' (or a final temperature to share with the user based on the received temperature sensor readings) 
can be scaled to any number of sensors, the TMP102 sensors being used can only be configured to use 4 different addresses. 
- This software is meant to work in tandem with a logging/display software on the host computer connected via USART to the microcontroller. While
  messages sent via USART can be viewed in a general serial monitor, this program relies on interacting with a software on the host computer to 
  configure it's RTC.

  To view one such program, feel free to check out the repository [here](https://github.com/CodeyBrody/TMP102RTSProjectLDSoftware.git).

  If you would like to create your own, feel free to take a look at the **Communication Protocol** section above to see how data communicated over USART is structured.

## License

This repository contains original application code as well as software components generated or provided by STMicroelectronics and other third parties.

Original application code is provided by the author. Vendor-provided and third-party components retain their respective copyrights and license terms. See the applicable license files and copyright notices included with those components.
