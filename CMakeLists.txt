cmake_minimum_required(VERSION 3.8)
project(bytev5)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add executable sources
add_executable(bytev5
   "bytev5.cpp" "bytev5.h"
"notDatabase/database.cpp" "notDatabase/database.h" "notDatabase/sqlite3.c" "notDatabase/sqlite3.h" "notDatabase/sqlite3ext.h" "notDatabase/SqliteDatabase.cpp" "notDatabase/SqliteDatabase.h" 
 "src/Application.cpp" "src/Application.h" "src/ConnectionPool/ConnectionPool.cpp" "src/ConnectionPool/ConnectionPool.h" "src/ConnectionPool/IConnectionPool.h" "src/DatabaseCon/DatabaseCon.cpp" "src/DatabaseCon/DatabaseCon.h" "src/DatabaseCon/IDatabaseCon.h" "src/NewcoinAddress/NewcoinAddress.cpp" "src/NewcoinAddress/NewcoinAddress.h" "src/Peer/IPeer.h" "src/Peer/Peer.cpp" "src/Peer/Peer.h" "src/UniqueNodeList/IUniqueNodeList.h" "src/UniqueNodeList/UniqueNodeList.cpp" "src/UniqueNodeList/UniqueNodeList.h" "src/UniqueNodeList/ValidatorSource.h" "src/Wallet/IWallet.h" "src/Wallet/Wallet.cpp" "src/Wallet/Wallet.h"
 "shared/base58.h" "shared/bignum.h" "shared/BitcoinUtil.cpp" "shared/BitcoinUtil.h" "shared/Conversion.cpp" "shared/Conversion.h" "shared/HttpsClient.cpp" "shared/HttpsClient.h" "shared/key.h" "shared/Log.cpp" "shared/Log.h" "shared/PackedMessage.cpp" "shared/PackedMessage.h" "shared/ParseSection.cpp" "shared/ParseSection.h" "shared/rfc1751.cpp" "shared/rfc1751.h" "shared/ScopedLock.h" "shared/SecureAllocator.h" "shared/Serializer.cpp" "shared/Serializer.h" "shared/types.h" "shared/uint256.h" "shared/utils.cpp" "shared/utils.h"
 "json/autolink.h" "json/config.h" "json/features.h" "json/forwards.h" "json/json.h" "json/json_batchallocator.h" "json/json_reader.cpp" "json/json_value.cpp" "json/json_writer.cpp" "json/reader.h" "json/value.h" "json/writer.h" 
)

# Find packages (use the system-installed libraries in WSL)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(boost_filesystem  REQUIRED CONFIG)
find_package(boost_asio  REQUIRED CONFIG)
#find_package(boost_test  REQUIRED CONFIG)

# Include directories
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})


# Link directories and libraries
target_link_libraries(bytev5 PRIVATE
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
    ${Protobuf_LIBRARIES}
    ${Boost_LIBRARIES}
    sqlite3
    boost_filesystem
    boost_format
    boost_interprocess
    boost_test
    boost_thread
    boost_utility
)

# Enable Protobuf generation
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/newcoin.proto)
target_sources(bytev5 PRIVATE ${PROTO_SRCS} ${PROTO_HDRS})
