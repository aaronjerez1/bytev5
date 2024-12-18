cmake_minimum_required(VERSION 3.8)
project(bytev5)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add executable sources
add_executable(bytev5
    "bytev5.cpp" "bytev5.h"

)

# Find packages (use the system-installed libraries in WSL)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)

# Include directories
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})

# Link directories and libraries
target_link_libraries(bytev5 PRIVATE
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
    ${Protobuf_LIBRARIES}
    sqlite3
    boost_asio
    boost_filesystem
    boost_format
    boost_interprocess
    boost_test
    boost_thread
    boost_utility
)

# Enable Protobuf generation
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/newcoin.proto)
#target_sources(bytev5 PRIVATE ${PROTO_SRCS} ${PROTO_HDRS})
