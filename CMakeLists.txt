# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp-s3-box-badge)

# Define the output path for the combined binary
set(COMBINED_BINARY ${CMAKE_BINARY_DIR}/combined_firmware.bin)

# Add a custom target for generating the combined firmware image
add_custom_target(
    combined_image
    COMMAND ${ESPTOOLPY} --chip esp32s3 merge_bin
        --flash_mode dio --flash_freq 80m --flash_size 16MB
        0x0 ${CMAKE_BINARY_DIR}/bootloader/bootloader.bin
        0x30000 ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
        0x8000 ${CMAKE_BINARY_DIR}/partition_table/partition-table.bin
        0x230000 ${CMAKE_BINARY_DIR}/storage.bin 
        -o ${COMBINED_BINARY}
    DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf
            ${CMAKE_BINARY_DIR}/bootloader/bootloader.bin
            ${CMAKE_BINARY_DIR}/partition_table/partition-table.bin
            ${CMAKE_BINARY_DIR}/storage.bin
    COMMENT "Generating combined firmware image: ${COMBINED_BINARY}"
)

# Define the output path for the combined binary
set(PARTIAL_BINARY ${CMAKE_BINARY_DIR}/partial_firmware.bin)

# Add a custom target for generating the combined firmware image
add_custom_target(
    partial_image
    COMMAND ${ESPTOOLPY} --chip esp32s3 merge_bin
        --flash_mode dio --flash_freq 80m --flash_size 16MB
        0x30000 ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
        0x8000 ${CMAKE_BINARY_DIR}/partition_table/partition-table.bin
        0x230000 ${CMAKE_BINARY_DIR}/storage.bin 
        -o ${PARTIAL_BINARY}
    DEPENDS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf
            ${CMAKE_BINARY_DIR}/bootloader/bootloader.bin
            ${CMAKE_BINARY_DIR}/partition_table/partition-table.bin
            ${CMAKE_BINARY_DIR}/storage.bin
    COMMENT "Generating combined firmware image: ${COMBINED_BINARY}"
)
