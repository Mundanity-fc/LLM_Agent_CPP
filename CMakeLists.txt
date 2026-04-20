cmake_minimum_required(VERSION 4.2)
project(LLM_Agent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)


add_executable(LLM_Agent main.cpp)

target_link_libraries(LLM_Agent PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::boost
        Boost::system
        Boost::filesystem
)
if(WIN32)
    target_link_libraries(LLM_Agent PRIVATE ws2_32 mswsock)
endif()