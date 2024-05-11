#ifndef BASE_CONVERT_HPP
#define BASE_CONVERT_HPP

#include "base_nodes.hpp"

// c++基础类型转换节点

// bool -> int
Node *SpawnBoolToIntNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "布尔值 转 整数", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "布尔值", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "整数", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        int int_value = value;
        node->Outputs[0].SetValue(int_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// int -> bool
Node *SpawnIntToBoolNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "整数 转 布尔值", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "整数", PinType::Int);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "布尔值", PinType::Bool);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        bool bool_value = value;
        node->Outputs[0].SetValue(bool_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// int -> float
Node *SpawnIntToFloatNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "整数 转 浮点数", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "整数", PinType::Int);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "浮点数", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        float float_value = static_cast<float>(value);
        node->Outputs[0].SetValue(float_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// float -> int
Node *SpawnFloatToIntNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "浮点数 转 整数", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "浮点数", PinType::Float, 0.0f);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "整数", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        int int_value = static_cast<int>(value);
        node->Outputs[0].SetValue(int_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// float -> bool
Node *SpawnFloatToBoolNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "浮点数 转 布尔值", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "浮点数", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "布尔值", PinType::Bool);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        bool bool_value = value;
        node->Outputs[0].SetValue(bool_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// bool -> float
Node *SpawnBoolToFloatNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "布尔值 转 浮点数", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "布尔值", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "浮点数", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        float float_value = value;
        node->Outputs[0].SetValue(float_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// string -> int
Node *SpawnStringToIntNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "文本 转 整数", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "文本", PinType::String);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "整数", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        int int_value = std::stoi(value);
        node->Outputs[0].SetValue(int_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// string -> float
Node *SpawnStringToFloatNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "文本 转 浮点数", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "文本", PinType::String);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "浮点数", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        float float_value = std::stof(value);
        node->Outputs[0].SetValue(float_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// string -> bool
Node *SpawnStringToBoolNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "文本 转 布尔值", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "文本", PinType::String);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "布尔值", PinType::Bool);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        bool bool_value = value == "true";
        node->Outputs[0].SetValue(bool_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// int -> string
Node *SpawnIntToStringNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "整数 转 文本", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "整数", PinType::Int);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "文本", PinType::String);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        std::string string_value = std::to_string(value);
        node->Outputs[0].SetValue(string_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// float -> string
Node *SpawnFloatToStringNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "浮点数 转 文本", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "浮点数", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "文本", PinType::String);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        std::string string_value = std::to_string(value);
        node->Outputs[0].SetValue(string_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// bool -> string
Node *SpawnBoolToStringNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "布尔值 转 文本", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "布尔值", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "文本", PinType::String);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = value;

        try_catch_block;

        std::string string_value = value ? "true" : "false";
        node->Outputs[0].SetValue(string_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static std::vector<std::pair<std::string, std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)>>> BaseConvertNodes = {
    {"布尔值 转 整数", SpawnBoolToIntNode},
    {"整数 转 布尔值", SpawnIntToBoolNode},
    {"整数 转 浮点数", SpawnIntToFloatNode},
    {"浮点数 转 整数", SpawnFloatToIntNode},
    {"浮点数 转 布尔值", SpawnFloatToBoolNode},
    {"布尔值 转 浮点数", SpawnBoolToFloatNode},
    {"文本 转 整数", SpawnStringToIntNode},
    {"文本 转 浮点数", SpawnStringToFloatNode},
    {"文本 转 布尔值", SpawnStringToBoolNode},
    {"整数 转 文本", SpawnIntToStringNode},
    {"浮点数 转 文本", SpawnFloatToStringNode},
    {"布尔值 转 文本", SpawnBoolToStringNode},
};

#endif // BASE_TYPE_HPP