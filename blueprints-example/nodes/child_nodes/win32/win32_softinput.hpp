#pragma once
#include "base_nodes.hpp"

Node *Spawn_EnumMouseClickTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
Node *Spawn_EnumMouseClickActionTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);

// software mouse click node
Node *Spawn_Win32_SoftInput_MouseClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// software mouse move node
Node *Spawn_Win32_SoftInput_MouseMove(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// software mouse wheel node
Node *Spawn_Win32_SoftInput_MouseWheel(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// software key press node
Node *Spawn_Win32_SoftInput_KeyPress(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// software key release node
Node *Spawn_Win32_SoftInput_KeyRelease(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);

static NodeWorldGlobal::FactoryGroupFunc_t Win32SoftInputNodes = {
    {"Win32 鼠标点击类型枚举", Spawn_EnumMouseClickTypeNode},
    {"Win32 鼠标点击动作枚举", Spawn_EnumMouseClickActionTypeNode},
    {"Win32 软件鼠标点击", Spawn_Win32_SoftInput_MouseClick},
    {"Win32 软件鼠标移动", Spawn_Win32_SoftInput_MouseMove},
    {"Win32 软件鼠标滚轮", Spawn_Win32_SoftInput_MouseWheel},
    {"Win32 软件键盘按下", Spawn_Win32_SoftInput_KeyPress},
    {"Win32 软件键盘释放", Spawn_Win32_SoftInput_KeyRelease},
};
