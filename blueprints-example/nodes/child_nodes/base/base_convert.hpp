#ifndef BASE_CONVERT_HPP
#define BASE_CONVERT_HPP

#include "base_nodes.hpp"

// c++基础类型转换节点

// bool -> int
Node *SpawnBoolToIntNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Bool To Int", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Int);
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
    m_Nodes.emplace_back(GetNextId(), "Int To Bool", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Int);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Bool);
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
    m_Nodes.emplace_back(GetNextId(), "Int To Float", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Int);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
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
    m_Nodes.emplace_back(GetNextId(), "Float To Int", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float, 0.0f);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Int);
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
    m_Nodes.emplace_back(GetNextId(), "Float To Bool", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Bool);
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
    m_Nodes.emplace_back(GetNextId(), "Bool To Float", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
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
    m_Nodes.emplace_back(GetNextId(), "String To Int", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::String);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Int);
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
    m_Nodes.emplace_back(GetNextId(), "String To Float", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::String);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
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
    m_Nodes.emplace_back(GetNextId(), "String To Bool", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::String);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Bool);
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
    m_Nodes.emplace_back(GetNextId(), "Int To String", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Int);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::String);
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
    m_Nodes.emplace_back(GetNextId(), "Float To String", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::String);
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
    m_Nodes.emplace_back(GetNextId(), "Bool To String", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseConvert;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::String);
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

static NodeWorldGlobal::FactoryGroupFunc_t BaseConvertNodes = {
    {"Bool To Int", SpawnBoolToIntNode},
    {"Int To Bool", SpawnIntToBoolNode},
    {"Int To Float", SpawnIntToFloatNode},
    {"Float To Int", SpawnFloatToIntNode},
    {"Float To Bool", SpawnFloatToBoolNode},
    {"Bool To Float", SpawnBoolToFloatNode},
    {"String To Int", SpawnStringToIntNode},
    {"String To Float", SpawnStringToFloatNode},
    {"String To Bool", SpawnStringToBoolNode},
    {"Int To String", SpawnIntToStringNode},
    {"Float To String", SpawnFloatToStringNode},
    {"Bool To String", SpawnBoolToStringNode},
};

#endif // BASE_TYPE_HPP