cmake_minimum_required(VERSION 3.10)
project(order_book)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER /opt/homebrew/bin/g++-15)
set(CMAKE_C_COMPILER /opt/homebrew/bin/gcc-15)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -OO")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(include)

set(SOURCES
    src/Order.cpp
    src/OrderBook.cpp
    src/Trade.cpp
    src/MatchingEngine.cpp
    src/main.cpp
)

set(HEADERS
    include/Order.h
    include/OrderBook.h
    include/Trade.h
    include/MatchingEngine.h
)

add_executable(orderbook ${SOURCES} ${HEADERS})

option(BUILD_TESTS "Build test executable" ON)

if(BUILD_TESTS)
    add_executable(orderbook_tests
        tests/OrderbookTest.cpp
        src/Order.cpp
            src/OrderBook.cpp
        src/Trade.cpp
        src/MatchingEngine.cpp
    )
endif()
