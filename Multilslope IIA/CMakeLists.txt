cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(multislope C CXX ASM)
set(CMAKE_C_STANDARD11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.c
)

pico_generate_pio_header(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/ms.pio
)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_pio
)
