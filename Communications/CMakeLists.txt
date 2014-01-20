cmake_minimum_required (VERSION 2.6)

# List of protobuf .proto's
set(LIBNETWORK_PROTOS
    protocol.proto
)

# List of source files
set(LIBNETWORK_SRCS
    Client.cpp
    Client.hpp
    ClientWrapper.cpp
    ClientWrapper.hpp
    ClientObserver.cpp
    ClientObserver.hpp
    Message.cpp
    Message.hpp
    ${LIBNETWORK_PROTOS}
)

# Use binary dir as include dir as protobuf headers will be generated there
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Protobuf generation
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${LIBNETWORK_PROTOS})

add_library(network ${LIBNETWORK_SRCS} ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(network ${Boost_LIBRARIES})
target_link_libraries(network ${PROTOBUF_LIBRARY})

