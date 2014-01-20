cmake_minimum_required (VERSION 2.6)
project (demo)

# Use C++11
set(MY_CXXFLAGS "-std=c++0x -lwiringPi")


set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall ${CMAKE_CXX_FLAGS_DEBUG} ${MY_CXXFLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MY_CXXFLAGS}")
set(CMAKE_BUILD_TYPE debug)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable all warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")

ENABLE_TESTING()

set(BOOSTCOMPONENTS system thread regex)

if(NOT ANDROID)
    set(BOOSTCOMPONENTS ${BOOSTCOMPONENTS} unit_test_framework)
endif()

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.49.0 REQUIRED COMPONENTS ${BOOSTCOMPONENTS})
include_directories(${Boost_INCLUDE_DIRS})

find_package(Protobuf 2.3.0 REQUIRED)

SET_SOURCE_FILES_PROPERTIES( Devices/NexaPlug/radio.h Devices/NexaPlug/radio.c Devices/NexaPlug/nexa.c Devices/NexaPlug/nexa.h PROPERTIES LANGUAGE CXX ) 

if(DEFINED ANDROID)
    add_definitions(-DANDROID)
endif()

if(DEFINED APPLE)
    add_definitions(-DAPPLE)
endif()

if(DEFINED UNIX)
    add_definitions(-DUNIX)
endif()

if(DEFINED WIN32)
    add_definitions(-DWINDOWS)
endif()

# TODO: Add -DLINUX check

set(UTIL_SRC
	Helpfunctions/helpfunctions.cpp
	Helpfunctions/helpfunctions.hh
	)

set(GPIO_SRCS
	GPIO/GPIO.cpp
	GPIO/GPIO.hh
	)
#)

set(SENSORS_SRCS
	Sensors/Thermometer/Thermometer.cpp
	Sensors/Thermometer/Thermometer.hh
	Sensors/Humidity/Humidity.cpp
	Sensors/Humidity/Humidity.hh
	Sensors/Switch/Switch.cpp
	Sensors/Switch/Switch.hh
	Helpfunctions/helpfunctions.cpp
	Helpfunctions/helpfunctions.hh
)

set(NEXA_SRCS
	Devices/NexaPlug/radio.h
	Devices/NexaPlug/radio.c
	Devices/NexaPlug/nexa.c
	Devices/NexaPlug/nexa.h
	Devices/NexaPlug/NexaPlug.hh
	Devices/NexaPlug/NexaPlug.cpp
)

set(DEVICES_SRCS
	Devices/Plugs/Plugs.cpp
	Devices/Plugs/Plugs.hh
	Devices/Speaker/Speaker.cpp
	Devices/Speaker/Speaker.hh
	Devices/Voice/voice.hh
	Devices/Voice/voice.cpp
)

	#set(COMM_SRCS
	#Communications/Communications.cpp
	#Communications/Communications.hh
	#)

# add_library(util ${UTIL_SRC})

# List of protobuf .proto's
set(LIBNETWORK_PROTOS
    Communications/protocol.proto
)

# List of source files
set(LIBNETWORK_SRCS
    Communications/Client.cpp
    Communications/Client.hpp
    Communications/ClientWrapper.cpp
    Communications/ClientWrapper.hpp
    Communications/ClientObserver.cpp
    Communications/ClientObserver.hpp
    Communications/Message.cpp
    Communications/Message.hpp
    ${LIBNETWORK_PROTOS}
)

# Use binary dir as include dir as protobuf headers will be generated there
# include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Protobuf generation
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${LIBNETWORK_PROTOS})


add_library(network ${LIBNETWORK_SRCS} ${PROTO_SRCS} ${PROTO_HDRS})
add_library(sensors ${SENSORS_SRCS})
add_library(nexa ${NEXA_SRCS})
add_library(devices ${DEVICES_SRCS})


#add_subdirectory(Communications)
#include_directories(Communications)

add_executable(demo demo.cpp)
add_executable(nexa_pair nexa_pair.cpp)

#target_link_libraries(rasp_prog network box)

# target_link_libraries(demo box)

target_link_libraries(demo sensors)
target_link_libraries(demo nexa)
target_link_libraries(demo network)

target_link_libraries(nexa_pair nexa)

#target_link_libraries(rasp_prog network)

# target_link_libraries(demo util)



