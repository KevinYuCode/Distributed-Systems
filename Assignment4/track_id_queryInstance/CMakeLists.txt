#
#

project(track_id LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.16)

# specify location of custom installed cyclone location
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    message(STATUS ">>> Linux")
    set (CMAKE_PREFIX_PATH /home/kevin/cyclone/install/lib/cmake)
    LIST(APPEND CMAKE_PROGRAM_PATH  "/home/kevin/cyclone/install/bin")
endif()
if(APPLE)
    message(STATUS ">>> Mac OS")
    set(CMAKE_PREFIX_PATH /Users/dean/Teaching/477/DDS/install/lib/cmake)
    LIST(APPEND CMAKE_PROGRAM_PATH  "/Users/dean/Teaching/477/DDS/install/bin")
endif()

# CXX standard at least 
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

if(NOT TARGET CycloneDDS-CXX::ddscxx)
  find_package(CycloneDDS-CXX REQUIRED)
endif()

idlcxx_generate(TARGET trackdata FILES track.idl WARNINGS no-implicit-extensibility)

add_executable(track_publisher trackpublisher.cpp)
add_executable(track_subscriber tracksubscriber.cpp)

# Link both executables to idl data type library and ddscxx.
target_link_libraries(track_publisher CycloneDDS-CXX::ddscxx trackdata)
target_link_libraries(track_subscriber CycloneDDS-CXX::ddscxx trackdata)

set_property(TARGET track_publisher PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
set_property(TARGET track_subscriber PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
