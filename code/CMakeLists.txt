cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)

set(PICO_PLATFORM rp2040)
set(BOARD pico_sdk)

include(pico_sdk_import.cmake)

# Run from SRAM
set(PICO_DEFAULT_BINARY_TYPE copy_to_ram)

set(PROJECT DOUCHELAG)

project(${PROJECT})

pico_sdk_init()

add_compile_options( -Ofast )

add_executable(${PROJECT})

pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)        

pico_add_extra_outputs(${PROJECT})

target_sources(${PROJECT} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/main.c ${CMAKE_CURRENT_SOURCE_DIR}/ssd1306.c)

target_link_libraries(${PROJECT} pico_stdlib hardware_i2c)

pico_enable_stdio_usb(DOUCHELAG 1) 
pico_enable_stdio_uart(DOUCHELAG 0) 
