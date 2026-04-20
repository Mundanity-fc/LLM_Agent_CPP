cmake_minimum_required(VERSION 4.2)
project(LLM_Agent)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)

add_executable(LLM_Agent main.cpp)
target_link_libraries(LLM_Agent PRIVATE OpenSSL::SSL OpenSSL::Crypto)
