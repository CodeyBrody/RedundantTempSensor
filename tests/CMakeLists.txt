cmake_minimum_required(VERSION 3.20)

project(RedundantTempSensorTests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(unit_tests
    unit/test_runner.c
    unit/determination_logic/test_determination_logic.c
    ../Core/App/determination_logic.c
    Unity-master/src/unity.c
    mocks/logging_mock.c
)

target_include_directories(unit_tests PRIVATE
    Unity-master/src
    ../Core/Inc
    fakes
)