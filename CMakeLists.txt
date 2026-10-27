cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set cross-compilers
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(tiva_smart_home C ASM)

set(CMAKE_C_STANDARD 11)

# Hardware FPU and Cortex-M4 flags (defined as a list without quotes around the whole thing)
set(CPU_FLAGS
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
)

add_compile_options(
        ${CPU_FLAGS}
        -Wall
        -Wextra
        -O0
        -g3
        -ffunction-sections
        -fdata-sections
)

# Linker flags
set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/tm4c123gh6pm.ld")
add_link_options(
        ${CPU_FLAGS}
        -T${LINKER_SCRIPT}
        -Wl,--gc-sections
        --specs=nosys.specs
        --specs=nano.specs
        -Wl,-Map=${CMAKE_PROJECT_NAME}.map
)
include_directories(
        inc
        src
)

# Grab all source files
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${CMAKE_PROJECT_NAME}.elf ${SOURCES})

# Post-build: print firmware memory size and emit .bin
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_SIZE} ${CMAKE_PROJECT_NAME}.elf
        COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
)