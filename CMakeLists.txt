cmake_minimum_required(VERSION 4.2)
project(LLM_Agent LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 启用单元测试
include(CTest)

# 启用 OpenSSL 与 Boost 库
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS json)

# 核心依赖库目标
add_library(LLM_Agent_Lib STATIC
        providers/Provider.h
        providers/OpenAICompatibleProvider.cpp
        providers/OpenAICompatibleProvider.h

        llm/HttpClient.cpp
        llm/HttpClient.h

        components/Chat.h
        components/Conversation.cpp
        components/Conversation.h
        components/ChatRequestBuilder.cpp
        components/ChatRequestBuilder.h
        components/StreamResponseAccumulator.cpp
        components/StreamResponseAccumulator.h
        components/ProviderComponents.h
        components/StreamComponents.h
        components/TokenComponents.h
        components/ReasoningComponent.h
)

# 核心依赖库位置
target_include_directories(LLM_Agent_Lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}
)

# 核心依赖库所需外部依赖
target_link_libraries(LLM_Agent_Lib PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::boost
        Boost::json
)

# Windows 额外配置
if(WIN32)
    target_link_libraries(LLM_Agent PRIVATE ws2_32 mswsock)
endif()

# 可执行文件目标
add_executable(LLM_Agent main.cpp)
# 连接核心依赖库
target_link_libraries(LLM_Agent PRIVATE LLM_Agent_Lib)


# 配置文件输出目录
set(LLM_AGENT_CONFIG_DIR
        "${PROJECT_BINARY_DIR}/config"
)
file(MAKE_DIRECTORY "${LLM_AGENT_CONFIG_DIR}")
# 配置文件
set(LLM_AGENT_CONFIG_FILES
        env.json
        env.json.example
)
# 文件复制
foreach(CONFIG_FILE IN LISTS LLM_AGENT_CONFIG_FILES)
    configure_file(
            "${PROJECT_SOURCE_DIR}/${CONFIG_FILE}"
            "${LLM_AGENT_CONFIG_DIR}/${CONFIG_FILE}"
            COPYONLY
    )
endforeach()


# 参数传递
target_compile_definitions(LLM_Agent
        PRIVATE
        LLM_AGENT_CONFIG_DIR="${LLM_AGENT_CONFIG_DIR}"
)


# 单元测试路径
if (BUILD_TESTING)
    add_subdirectory(Test)
endif()