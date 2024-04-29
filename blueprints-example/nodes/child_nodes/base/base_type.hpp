#ifndef BASE_TYPE_HPP
#define BASE_TYPE_HPP

#include "base_nodes.hpp"

// c++基础类型节点

// bool 类型节点
Node *SpawnBoolNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Bool", ImColor(255, 128, 128));
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Bool, false);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

// int 类型节点
Node *SpawnIntNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int", ImColor(255, 128, 128));
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Int, 0);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

// float 类型节点
Node *SpawnFloatNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Float", ImColor(255, 128, 128));
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float, 0.0f);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

// string 类型节点
Node *SpawnStringNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "String", ImColor(255, 128, 128));
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::String, "");

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

static NodeWorldGlobal::FactoryGroupFunc_t BaseTypeNodes = {
    {"Bool", SpawnBoolNode},
    {"Int", SpawnIntNode},
    {"Float", SpawnFloatNode},
    {"String", SpawnStringNode},
};
#endif // BASE_TYPE_HPP