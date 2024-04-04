#
#

project(assign4 LANGUAGES C CXX)
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

# generation of code from idl files. One line per type
idlcxx_generate(TARGET statedata FILES statekey.idl WARNINGS no-implicit-extensibility)
idlcxx_generate(TARGET controlTransfer FILES controlTransfer.idl WARNINGS no-implicit-extensibility)

add_executable(genData genData.cpp aircraft.cpp)
add_executable(query query.cpp aircraft.cpp)
add_executable(center center.cpp aircraft.cpp)
add_executable(apartDepart apartDepart.cpp aircraft.cpp)
add_executable(flights flights.cpp aircraft.cpp)
add_executable(subscriber subscriber.cpp)

# Link both executables to idl data type library and ddscxx.
target_link_libraries(genData CycloneDDS-CXX::ddscxx statedata controlTransfer)
target_link_libraries(query CycloneDDS-CXX::ddscxx statedata controlTransfer)
target_link_libraries(center CycloneDDS-CXX::ddscxx statedata controlTransfer)
target_link_libraries(apartDepart CycloneDDS-CXX::ddscxx statedata controlTransfer)
target_link_libraries(flights CycloneDDS-CXX::ddscxx statedata controlTransfer)
target_link_libraries(subscriber CycloneDDS-CXX::ddscxx statedata controlTransfer)

set_property(TARGET genData PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
set_property(TARGET query PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
set_property(TARGET center PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
set_property(TARGET apartDepart PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
set_property(TARGET flights PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
set_property(TARGET subscriber PROPERTY CXX_STANDARD ${cyclonedds_cpp_std_to_use})
