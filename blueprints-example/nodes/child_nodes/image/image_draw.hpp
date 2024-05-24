#pragma once
#include "base_nodes.hpp"

// draw line
Node *Spawn_ImageOperator_DrawLine(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// draw rectangle
Node *Spawn_ImageOperator_DrawRectangle(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// draw circle
Node *Spawn_ImageOperator_DrawCircle(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// draw ellipse
Node *Spawn_ImageOperator_DrawEllipse(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// draw text
Node *Spawn_ImageOperator_DrawText(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
static NodeWorldGlobal::FactoryGroupFunc_t ImageDrawNodes = {
    {"画线", Spawn_ImageOperator_DrawLine},
    {"画矩形", Spawn_ImageOperator_DrawRectangle},
    {"画圆", Spawn_ImageOperator_DrawCircle},
    {"画椭圆", Spawn_ImageOperator_DrawEllipse},
    {"画文本", Spawn_ImageOperator_DrawText},
};