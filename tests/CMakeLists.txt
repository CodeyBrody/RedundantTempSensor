cmake_minimum_required(VERSION 3.20)

project(RedundantTempSensorTests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(unit_tests
    unit/test_runner.c
    unit/determination_logic/test_findAverageValidTemp.c
    unit/determination_logic/test_outlier_detection.c
    unit/determination_logic/test_aboveOOB_detection.c
    unit/determination_logic/test_missing_sensors.c
    unit/usart/test_usart_commands.c
    unit/sensor/test_sensor.c
    ../Core/App/determination_logic.c
    ../Core/App/sensor.c
    ../Core/App/hardware.c
    ../Core/App/interrupt_handlers.c
    Unity-master/src/unity.c
    mocks/logging_mock.c
    fakes/stm32l4xx_hal.c
)

target_include_directories(unit_tests PRIVATE
    Unity-master/src
    ../Core/Inc
    fakes
    
)


add_executable(rtc_tests
    unit/rtc/test_rtc.c
    ../Core/App/rtc.c
    Unity-master/src/unity.c
    fakes/stm32l4xx_hal.c
)

target_include_directories(rtc_tests PRIVATE
    Unity-master/src
    ../Core/Inc
    fakes
)