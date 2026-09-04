#pragma once

// 推理输出类型
enum class ReasoningOutputKind {
    None,
    Summary,
    Trace
};

// 推理输出结构
struct ReasoningOutput {
    // 类型
    ReasoningOutputKind kind{ReasoningOutputKind::None};
    // 文本
    std::string text{};
};