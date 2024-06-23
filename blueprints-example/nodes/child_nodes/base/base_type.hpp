#ifndef BASE_TYPE_HPP
#define BASE_TYPE_HPP

#include "base_nodes.hpp"

// c++基础类型节点

// bool 类型节点
Node *SpawnBoolNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "布尔值", ImColor(255, 128, 128));
    m_Nodes.back().Type = NodeType::Simple;
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "布尔值", PinType::Bool, false);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

// int 类型节点
Node *SpawnIntNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "整数", ImColor(255, 128, 128));
    m_Nodes.back().Type = NodeType::Simple;
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "浮点数", PinType::Int, 0);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

// float 类型节点
Node *SpawnFloatNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "浮点数", ImColor(255, 128, 128));
    m_Nodes.back().Type = NodeType::Simple;
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "浮点数", PinType::Float, 0.0f);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

// string 类型节点
Node *SpawnStringNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "文本", ImColor(255, 128, 128));
    m_Nodes.back().Type = NodeType::Simple;
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "文本", PinType::String, "");

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

static NodeWorldGlobal::FactoryGroupFunc_t BaseTypeNodes = {
    {"布尔值", SpawnBoolNode},
    {"整数", SpawnIntNode},
    {"浮点数", SpawnFloatNode},
    {"文本", SpawnStringNode},
};

static std::vector<std::pair<std::string, factory_func_t>> BaseTypeNodesFactorys = {
    {"基础类型/布尔值", SpawnBoolNode},
    {"基础类型/整数", SpawnIntNode},
    {"基础类型/浮点数", SpawnFloatNode},
    {"基础类型/文本", SpawnStringNode},
};

#endif // BASE_TYPE_HPP