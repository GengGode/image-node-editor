#ifndef BASE_OPERATION_HPP
#define BASE_OPERATION_HPP

#include "base_nodes.hpp"

// c++基础类型操作节点

// bool 类型操作节点

// bool 取反
Node *SpawnBoolNotNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Bool Not", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "In", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Bool);
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

        bool result_value = !value;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// bool 与
Node *SpawnBoolAndNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Bool And", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Bool);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        bool result_value = a && b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// bool 或
Node *SpawnBoolOrNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Bool Or", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Bool);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        bool result_value = a || b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// bool 异或
Node *SpawnBoolXorNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Bool Xor", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Bool);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        bool result_value = a ^ b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// 8bool to int
Node *SpawnBytesToIntNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Byte To Int", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Byte 0", PinType::Bool, 0);
    node.Inputs.emplace_back(GetNextId(), "Byte 1", PinType::Bool, 0);
    node.Inputs.emplace_back(GetNextId(), "Byte 2", PinType::Bool, 0);
    node.Inputs.emplace_back(GetNextId(), "Byte 3", PinType::Bool, 0);
    node.Inputs.emplace_back(GetNextId(), "Byte 4", PinType::Bool, 0);
    node.Inputs.emplace_back(GetNextId(), "Byte 5", PinType::Bool, 0);
    node.Inputs.emplace_back(GetNextId(), "Byte 6", PinType::Bool, 0);
    node.Inputs.emplace_back(GetNextId(), "Byte 7", PinType::Bool, 0);
    node.Outputs.emplace_back(GetNextId(), "Int", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::array<bool, 8> bytes;
        for (int i = 0; i < 8; i++)
        {
            auto result = get_value(graph, node->Inputs[i], bytes[i]);
            if (result.has_error())
                return result;
        }

        // Display image
        for (int i = 0; i < 8; i++)
        {
            node->Inputs[i].Value = bytes[i];
        }

        try_catch_block;

        int result_value = 0;
        for (int i = 0; i < 8; i++)
        {
            result_value |= (bytes[i] << i);
        }
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// int 类型操作节点

// int 加法
Node *SpawnIntAddNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Add", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        int result_value = a + b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// int 减法
Node *SpawnIntSubtractNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Subtract", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        int result_value = a - b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// int 乘法
Node *SpawnIntMultiplyNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Multiply", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        int result_value = a * b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// int 除法
Node *SpawnIntDivideNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Divide", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        if (b == 0)
        {
            return ExecuteResult::ErrorPin(node->Inputs[1].ID, "Division by zero");
        }

        int result_value = a / b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// int 取余
Node *SpawnIntModuloNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Modulo", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        if (b == 0)
        {
            return ExecuteResult::ErrorPin(node->Inputs[1].ID, "Division by zero");
        }

        int result_value = a % b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// int 比较
Node *SpawnIntCompareNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Compare", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Equal", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Not Equal", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Less", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Less or Equal", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Greater", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Greater or Equal", PinType::Bool);
    node.Outputs[0].app = app;
    node.Outputs[1].app = app;
    node.Outputs[2].app = app;
    node.Outputs[3].app = app;
    node.Outputs[4].app = app;
    node.Outputs[5].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        bool equal = a == b;
        bool not_equal = a != b;
        bool less = a < b;
        bool less_or_equal = a <= b;
        bool greater = a > b;
        bool greater_or_equal = a >= b;

        node->Outputs[0].SetValue(equal);
        node->Outputs[1].SetValue(not_equal);
        node->Outputs[2].SetValue(less);
        node->Outputs[3].SetValue(less_or_equal);
        node->Outputs[4].SetValue(greater);
        node->Outputs[5].SetValue(greater_or_equal);

        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// int 乘方
Node *SpawnIntPowerNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Power", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Base", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Exponent", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int base, exponent;
        auto result_base = get_value(graph, node->Inputs[0], base);
        if (result_base.has_error())
            return result_base;

        auto result_exponent = get_value(graph, node->Inputs[1], exponent);
        if (result_exponent.has_error())
            return result_exponent;

        // Display image
        node->Inputs[0].Value = base;
        node->Inputs[1].Value = exponent;

        try_catch_block;

        int result_value = 1;
        for (int i = 0; i < exponent; i++)
        {
            result_value *= base;
        }
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// int 乘浮点
Node *SpawnIntMultiplyFloatNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Multiply Float", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Int", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Float", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a;
        float b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        float result_value = a * b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// int 除浮点
Node *SpawnIntDivideFloatNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int Divide Float", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Int", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Float", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a;
        float b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        if (b == 0)
        {
            return ExecuteResult::ErrorPin(node->Inputs[1].ID, "Division by zero");
        }

        float result_value = a / b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// float 类型操作节点

// float 加法
Node *SpawnFloatAddNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Float Add", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        float result_value = a + b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// float 减法
Node *SpawnFloatSubtractNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Float Subtract", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        float result_value = a - b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// float 乘法
Node *SpawnFloatMultiplyNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Float Multiply", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        float result_value = a * b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// float 除法
Node *SpawnFloatDivideNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Float Divide", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        if (b == 0)
        {
            return ExecuteResult::ErrorPin(node->Inputs[1].ID, "Division by zero");
        }

        float result_value = a / b;
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// float 取余
Node *SpawnFloatModuloNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Float Modulo", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        if (b == 0)
        {
            return ExecuteResult::ErrorPin(node->Inputs[1].ID, "Division by zero");
        }

        float result_value = std::fmod(a, b);
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// float 比较
Node *SpawnFloatCompareNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Float Compare", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Equal", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Not Equal", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Less", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Less or Equal", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Greater", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "Greater or Equal", PinType::Bool);
    node.Outputs[0].app = app;
    node.Outputs[1].app = app;
    node.Outputs[2].app = app;
    node.Outputs[3].app = app;
    node.Outputs[4].app = app;
    node.Outputs[5].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

        // Display image
        node->Inputs[0].Value = a;
        node->Inputs[1].Value = b;

        try_catch_block;

        bool equal = a == b;
        bool not_equal = a != b;
        bool less = a < b;
        bool less_or_equal = a <= b;
        bool greater = a > b;
        bool greater_or_equal = a >= b;

        node->Outputs[0].SetValue(equal);
        node->Outputs[1].SetValue(not_equal);
        node->Outputs[2].SetValue(less);
        node->Outputs[3].SetValue(less_or_equal);
        node->Outputs[4].SetValue(greater);
        node->Outputs[5].SetValue(greater_or_equal);

        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// float 乘方
Node *SpawnFloatPowerNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Float Power", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Base", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "Exponent", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float base, exponent;
        auto result_base = get_value(graph, node->Inputs[0], base);
        if (result_base.has_error())
            return result_base;

        auto result_exponent = get_value(graph, node->Inputs[1], exponent);
        if (result_exponent.has_error())
            return result_exponent;

        // Display image
        node->Inputs[0].Value = base;
        node->Inputs[1].Value = exponent;

        try_catch_block;

        float result_value = std::pow(base, exponent);
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static std::vector<std::pair<std::string, std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)>>> BaseOperationNodes = {

    {"Bool Not", SpawnBoolNotNode},
    {"Bool And", SpawnBoolAndNode},
    {"Bool Or", SpawnBoolOrNode},
    {"Bool Xor", SpawnBoolXorNode},
    {"Bytes To Int", SpawnBytesToIntNode},

    {"Int Add", SpawnIntAddNode},
    {"Int Subtract", SpawnIntSubtractNode},
    {"Int Multiply", SpawnIntMultiplyNode},
    {"Int Divide", SpawnIntDivideNode},
    {"Int Modulo", SpawnIntModuloNode},
    {"Int Compare", SpawnIntCompareNode},
    {"Int Power", SpawnIntPowerNode},
    {"Int Multiply Float", SpawnIntMultiplyFloatNode},
    {"Int Divide Float", SpawnIntDivideFloatNode},

    {"Float Add", SpawnFloatAddNode},
    {"Float Subtract", SpawnFloatSubtractNode},
    {"Float Multiply", SpawnFloatMultiplyNode},
    {"Float Divide", SpawnFloatDivideNode},
    {"Float Modulo", SpawnFloatModuloNode},
    {"Float Compare", SpawnFloatCompareNode},
    {"Float Power", SpawnFloatPowerNode},

    //{"String Concatenate", SpawnStringConcatenateNode},
    //{"String Length", SpawnStringLengthNode},
    //{"String Substring", SpawnStringSubstringNode},
    //{"String Find", SpawnStringFindNode},
    //{"String Replace", SpawnStringReplaceNode},
    //{"String Remove", SpawnStringRemoveNode},
    //{"String To Upper", SpawnStringToUpperNode},
    //{"String To Lower", SpawnStringToLowerNode},
    //{"String Trim", SpawnStringTrimNode},
    //{"String Split", SpawnStringSplitNode}
};

#endif // BASE_OPERATION_NODES_H