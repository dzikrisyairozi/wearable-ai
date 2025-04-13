cmake_minimum_required(VERSION 3.12)
project(wearable)

# Options
option(USE_FIFO "Using FIFO scheduler" OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(USE_FIFO)
    target_compile_definitions(icast PUBLIC USE_FIFO)
endif()

# Add executable
add_executable(hw_io src/hw_io.cpp src/wearable.cpp)
add_executable(middleware src/middleware.cpp src/wearable.cpp)
