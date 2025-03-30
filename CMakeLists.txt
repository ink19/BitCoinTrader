cmake_minimum_required(VERSION 3.10)

project(BitCoinTrader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio beast url)
find_package(glog REQUIRED)
find_package(cryptopp REQUIRED)

include_directories(common/include)
include_directories(market/okx)
file(GLOB_RECURSE COMMON_SRC_LIST "common/**/*.cpp")
file(GLOB_RECURSE OKX_SRC_LIST "market/**/*.cpp")

add_executable(BitCoinTrader main.cpp ${COMMON_SRC_LIST} ${OKX_SRC_LIST})

target_link_libraries(BitCoinTrader
    PRIVATE
    fmt::fmt
    OpenSSL::SSL
    OpenSSL::Crypto
    Boost::asio
    Boost::beast
    Boost::url
    glog::glog
    cryptopp::cryptopp
)
