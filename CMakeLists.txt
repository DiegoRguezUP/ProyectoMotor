cmake_minimum_required(VERSION 3.16)

# Only add the components we actually need
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_SOURCE_DIR}/drivers/HAL
    $ENV{IDF_PATH}/examples/common_components/protocol_examples_common
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(EncoderESP)
