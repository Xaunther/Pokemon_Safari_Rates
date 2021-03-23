cmake_minimum_required(VERSION 3.0.0)
project(Safari_rates VERSION 0.1.0)

include(CTest)
enable_testing()

string(APPEND CMAKE_CXX_FLAGS " -Wall -std=c++17")

add_executable(Safari_rates main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
