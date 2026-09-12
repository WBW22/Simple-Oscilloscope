cmake_minimum_required(VERSION 3.22)

#
# 本工程提供 <b>两套并行的构建系统</b>,源码完全共用,互不影响:
#   - Keil MDK-ARM (ARMCC)  : MDK-ARM/Simple-Oscilloscope.uvprojx
#   - CMake + Ninja + GCC   : 本文件(需要 arm-none-eabi-gcc 在 PATH 中)
#
# 本文件由 STM32CubeMX 的 CMake 工程模板改写而来,
# 结构对齐 ST 官方模板,便于和 CubeMX 重新生成的工程对比。
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Simple-Oscilloscope)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # --- 外设驱动 ---
    Hardware/ADC/adc.c
    Hardware/DMA/dma.c
    Hardware/FREQ/freq.c
    Hardware/KEY/key.c
    Hardware/LED/led.c
    Hardware/TFT/tft.c
    Hardware/TFT/tft_init.c
    Hardware/TIMER/timer.c
    # --- 系统层 ---
    SYSTEM/delay/delay.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Hardware/ADC
    Hardware/DMA
    Hardware/FREQ
    Hardware/KEY
    Hardware/LED
    Hardware/TFT
    Hardware/TIMER
    SYSTEM/delay
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# ---------------------------------------------------------------------------
# 生成烧录用的 .hex(GCC 默认只产出 .elf,烧录工具一般吃 .hex)
# ---------------------------------------------------------------------------
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
            $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/${CMAKE_PROJECT_NAME}.hex
    COMMENT "生成 ${CMAKE_PROJECT_NAME}.hex"
)

# ---------------------------------------------------------------------------
# 顺带打印一下体积,方便和 Keil 的 Program Size 对照
# ---------------------------------------------------------------------------
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMENT "固件体积"
)
