find_package(GTest CONFIG REQUIRED)

include(GoogleTest)

add_executable(LLM_Agent_Unit_Tests
        SSETest.cpp
)

target_link_libraries(LLM_Agent_Unit_Tests
        PRIVATE
        LLM_Agent_Lib
        GTest::gtest_main
)

# 参数传递
target_compile_definitions(LLM_Agent_Unit_Tests
        PRIVATE
        LLM_AGENT_CONFIG_DIR="${LLM_AGENT_CONFIG_DIR}"
)

gtest_discover_tests(LLM_Agent_Unit_Tests
        DISCOVERY_MODE PRE_TEST
)