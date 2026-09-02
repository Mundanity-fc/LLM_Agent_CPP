cmake_minimum_required(VERSION 4.2)
project(LLM_Agent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)


add_executable(LLM_Agent
        main.cpp
        providers/Provider.h
        providers/OpenAICompatibleProvider.cpp
        providers/OpenAICompatibleProvider.h
        llm/HttpClient.cpp
        llm/HttpClient.h
        components/ProviderConfig.h
        components/Chat.h
        components/Conversation.cpp
        components/Conversation.h
        components/ChatRequestBuilder.cpp
        components/ChatRequestBuilder.h
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