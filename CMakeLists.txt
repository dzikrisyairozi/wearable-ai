cmake_minimum_required(VERSION 3.12)
project(wearable)

# Find PortAudio using pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(PORTAUDIO REQUIRED portaudio-2.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${PORTAUDIO_INCLUDE_DIRS})

link_directories(${PORTAUDIO_LIBRARY_DIRS})

if(USE_FIFO)
    target_compile_definitions(icast PUBLIC USE_FIFO)
endif()

# Add executable
add_executable(hw_io src/hw_io.cpp src/wearable.cpp)
add_executable(middleware src/middleware.cpp src/wearable.cpp)

target_link_libraries(middleware ${PORTAUDIO_LIBRARIES})
