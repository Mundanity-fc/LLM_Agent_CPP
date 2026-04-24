cmake_minimum_required(VERSION 4.2)
project(LLM_Agent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)


add_executable(LLM_Agent
        main.cpp
        llm/LLM_Client.cpp
        components/Message.cpp
        providers/deepseek.cpp
        providers/deepseek.h
        providers/openrouter.cpp
        providers/openrouter.h
        providers/google.cpp
        providers/google.h
)

target_link_libraries(LLM_Agent PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::boost
        Boost::json
)
if(WIN32)
    target_link_libraries(LLM_Agent PRIVATE ws2_32 mswsock)
endif()