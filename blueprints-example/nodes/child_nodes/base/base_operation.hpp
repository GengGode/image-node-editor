#ifndef BASE_OPERATION_HPP
#define BASE_OPERATION_HPP

#include "base_nodes.hpp"

// c++基础类型操作节点

// bool 类型操作节点

// bool 取反
Node *SpawnBoolNotNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "布尔值 非", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "In", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Bool);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool value;
        get_value(graph, node->Inputs[0], value);

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
    m_Nodes.emplace_back(GetNextId(), "布尔值 与", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Bool);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "布尔值 或", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Bool);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "布尔值 异或", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Bool);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        bool a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "比特数组 转 整数", ImColor(255, 128, 128));
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

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::array<bool, 8> bytes;
        for (int i = 0; i < 8; i++)
        {
            get_value(graph, node->Inputs[i], bytes[i]);
        }

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
    m_Nodes.emplace_back(GetNextId(), "整数 加", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "整数 减", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "整数 乘", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "整数 除", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "整数 取余", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "整数 比较", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "相等", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "不相等", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "小于", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "小于等于", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "大于", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "大于等于", PinType::Bool);

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
    m_Nodes.emplace_back(GetNextId(), "整数 乘方", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Base", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Exponent", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Int);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int base, exponent;
        auto result_base = get_value(graph, node->Inputs[0], base);
        if (result_base.has_error())
            return result_base;

        auto result_exponent = get_value(graph, node->Inputs[1], exponent);
        if (result_exponent.has_error())
            return result_exponent;

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
    m_Nodes.emplace_back(GetNextId(), "整数 乘 浮点数", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Int", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Float", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);

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
    m_Nodes.emplace_back(GetNextId(), "整数 除 浮点数", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Int", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Float", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);

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
    m_Nodes.emplace_back(GetNextId(), "浮点数 加", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "浮点数 减", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "浮点数 乘", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "浮点数 除", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "浮点数 取余", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float a, b;
        auto result_a = get_value(graph, node->Inputs[0], a);
        if (result_a.has_error())
            return result_a;

        auto result_b = get_value(graph, node->Inputs[1], b);
        if (result_b.has_error())
            return result_b;

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
    m_Nodes.emplace_back(GetNextId(), "浮点数 比较", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "相等", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "不相等", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "小于", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "小于等于", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "大于", PinType::Bool);
    node.Outputs.emplace_back(GetNextId(), "大于等于", PinType::Bool);

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
    m_Nodes.emplace_back(GetNextId(), "浮点数 乘方", ImColor(255, 128, 128));
    auto &node = m_Nodes.back();
    node.Type = NodeType::BaseOperation;
    node.Inputs.emplace_back(GetNextId(), "Base", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "Exponent", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Out", PinType::Float);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        float base, exponent;
        auto result_base = get_value(graph, node->Inputs[0], base);
        if (result_base.has_error())
            return result_base;

        auto result_exponent = get_value(graph, node->Inputs[1], exponent);
        if (result_exponent.has_error())
            return result_exponent;

        try_catch_block;

        float result_value = std::pow(base, exponent);
        node->Outputs[0].SetValue(result_value);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t BaseOperationNodes = {

    {"布尔值 非", SpawnBoolNotNode},
    {"布尔值 与", SpawnBoolAndNode},
    {"布尔值 或", SpawnBoolOrNode},
    {"布尔值 异或", SpawnBoolXorNode},
    {"比特数组 转 整数", SpawnBytesToIntNode},

    {"整数 加", SpawnIntAddNode},
    {"整数 减", SpawnIntSubtractNode},
    {"整数 乘", SpawnIntMultiplyNode},
    {"整数 除", SpawnIntDivideNode},
    {"整数 取余", SpawnIntModuloNode},
    {"整数 比较", SpawnIntCompareNode},
    {"整数 乘方", SpawnIntPowerNode},
    {"整数 乘 浮点数", SpawnIntMultiplyFloatNode},
    {"整数 除 浮点数", SpawnIntDivideFloatNode},

    {"浮点数 加", SpawnFloatAddNode},
    {"浮点数 减", SpawnFloatSubtractNode},
    {"浮点数 乘", SpawnFloatMultiplyNode},
    {"浮点数 除", SpawnFloatDivideNode},
    {"浮点数 取余", SpawnFloatModuloNode},
    {"浮点数 比较", SpawnFloatCompareNode},
    {"浮点数 乘方", SpawnFloatPowerNode},
};

static std::vector<std::pair<std::string, factory_func_t>> BaseOperationNodesFactorys ={
    {"基础类型/计算/布尔值/非", SpawnBoolNotNode},
    {"基础类型/计算/布尔值/与", SpawnBoolAndNode},
    {"基础类型/计算/布尔值/或", SpawnBoolOrNode},
    {"基础类型/计算/布尔值/异或", SpawnBoolXorNode},
    {"基础类型/计算/比特数组/转 整数", SpawnBytesToIntNode},
    {"基础类型/计算/整数/加", SpawnIntAddNode},
    {"基础类型/计算/整数/减", SpawnIntSubtractNode},
    {"基础类型/计算/整数/乘", SpawnIntMultiplyNode},
    {"基础类型/计算/整数/除", SpawnIntDivideNode},
    {"基础类型/计算/整数/取余", SpawnIntModuloNode},
    {"基础类型/计算/整数/比较", SpawnIntCompareNode},
    {"基础类型/计算/整数/乘方", SpawnIntPowerNode},
    {"基础类型/计算/整数/乘 浮点数", SpawnIntMultiplyFloatNode},
    {"基础类型/计算/整数/除 浮点数", SpawnIntDivideFloatNode},
    {"基础类型/计算/浮点数/加", SpawnFloatAddNode},
    {"基础类型/计算/浮点数/减", SpawnFloatSubtractNode},
    {"基础类型/计算/浮点数/乘", SpawnFloatMultiplyNode},
    {"基础类型/计算/浮点数/除", SpawnFloatDivideNode},
    {"基础类型/计算/浮点数/取余", SpawnFloatModuloNode},
    {"基础类型/计算/浮点数/比较", SpawnFloatCompareNode},
    {"基础类型/计算/浮点数/乘方", SpawnFloatPowerNode},
};

#endif // BASE_OPERATION_NODES_H