#pragma once

#include "base_nodes.hpp"

// 字符串处理节点

// 字符串长度
Node *Spawn_StringOperator_Length(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "字符串长度");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文本", std::string(""));
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "长度");

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string str;
        get_value(graph, node->Inputs[0], str);

        try_catch_block
        {
            node->Outputs[0].Value = static_cast<int>(str.length());
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// 字符串连接
Node *Spawn_StringOperator_Concatenate(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "字符串连接");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文本", std::string(""));
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文本", std::string(""));
    node.Outputs.emplace_back(GetNextId(), PinType::String);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string str1, str2;
        get_value(graph, node->Inputs[0], str1);
        get_value(graph, node->Inputs[1], str2);

        try_catch_block
        {
            node->Outputs[0].Value = str1 + str2;
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// 字符串分割
Node *Spawn_StringOperator_Split(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "字符串分割");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文本", std::string(""));
    node.Inputs.emplace_back(GetNextId(), PinType::String, "分隔符", std::string(","));
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "分割数量");
    node.Outputs.emplace_back(GetNextId(), PinType::Array, "分割结果");

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string str, delimiter;
        get_value(graph, node->Inputs[0], str);
        get_value(graph, node->Inputs[1], delimiter);

        try_catch_block
        {
            std::vector<std::string> result;
            size_t last = 0;
            size_t index = str.find(delimiter);
            while (index != std::string::npos)
            {
                result.push_back(str.substr(last, index - last));
                last = index + delimiter.length();
                index = str.find(delimiter, last);
            }
            if (last < str.length())
                result.push_back(str.substr(last));

            node->Outputs[0].Value = static_cast<int>(result.size());
            // node->Outputs[1].Value = result;
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// 字符串替换
Node *Spawn_StringOperator_Replace(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "字符串替换");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文本", std::string(""));
    node.Inputs.emplace_back(GetNextId(), PinType::String, "查找", std::string(""));
    node.Inputs.emplace_back(GetNextId(), PinType::String, "替换", std::string(""));
    node.Outputs.emplace_back(GetNextId(), PinType::String);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string str, find, replace;
        get_value(graph, node->Inputs[0], str);
        get_value(graph, node->Inputs[1], find);
        get_value(graph, node->Inputs[2], replace);

        try_catch_block
        {
            size_t pos = 0;
            while ((pos = str.find(find, pos)) != std::string::npos)
            {
                str.replace(pos, find.length(), replace);
                pos += replace.length();
            }
            node->Outputs[0].Value = str;
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// 字符串查找
Node *Spawn_StringOperator_Find(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "字符串查找");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文本", std::string(""));
    node.Inputs.emplace_back(GetNextId(), PinType::String, "查找", std::string(""));
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "位置");

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string str, find;
        get_value(graph, node->Inputs[0], str);
        get_value(graph, node->Inputs[1], find);

        try_catch_block
        {
            node->Outputs[0].Value = static_cast<int>(str.find(find));
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// 字符串截取
Node *Spawn_StringOperator_Substring(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "字符串截取");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文本", std::string(""));
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "起始位置", 0);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "长度", 0);
    node.Outputs.emplace_back(GetNextId(), PinType::String);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string str;
        int start, length;
        get_value(graph, node->Inputs[0], str);
        get_value(graph, node->Inputs[1], start);
        get_value(graph, node->Inputs[2], length);

        try_catch_block
        {
            node->Outputs[0].Value = str.substr(start, length);
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t BaseStringNodes = {
    {"字符串长度", Spawn_StringOperator_Length},
    {"字符串连接", Spawn_StringOperator_Concatenate},
    {"字符串分割", Spawn_StringOperator_Split},
    {"字符串替换", Spawn_StringOperator_Replace},
    {"字符串查找", Spawn_StringOperator_Find},
    {"字符串截取", Spawn_StringOperator_Substring},
};

static std::vector<std::pair<std::string, factory_func_t>> BaseStringNodesFactorys = {
    {"字符串/字符串长度", Spawn_StringOperator_Length},
    {"字符串/字符串连接", Spawn_StringOperator_Concatenate},
    {"字符串/字符串分割", Spawn_StringOperator_Split},
    {"字符串/字符串替换", Spawn_StringOperator_Replace},
    {"字符串/字符串查找", Spawn_StringOperator_Find},
    {"字符串/字符串截取", Spawn_StringOperator_Substring},
};