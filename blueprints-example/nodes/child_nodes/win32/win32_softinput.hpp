#pragma once
#include "base_nodes.hpp"

Node *Spawn_EnumMouseClickTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
Node *Spawn_EnumMouseClickActionTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
Node *Spawn_EnumKeyContorlTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
Node *Spawn_EnumKeyClickActionTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);

// software mouse click node
Node *Spawn_Win32_SoftInput_MouseClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// software mouse move node
Node *Spawn_Win32_SoftInput_MouseMove(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// software mouse wheel node
Node *Spawn_Win32_SoftInput_MouseWheel(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// software key action node
Node *Spawn_Win32_SoftInput_KeyClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);

// post message mouse click node
Node *Spawn_Win32_PostMessage_MouseClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// post message mouse move node
Node *Spawn_Win32_PostMessage_MouseMove(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// post message mouse wheel node
Node *Spawn_Win32_PostMessage_MouseWheel(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// post message key action node
Node *Spawn_Win32_PostMessage_KeyClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);

static NodeWorldGlobal::FactoryGroupFunc_t Win32SoftInputNodes = {
    {"Win32 鼠标点击类型枚举", Spawn_EnumMouseClickTypeNode},
    {"Win32 鼠标点击动作枚举", Spawn_EnumMouseClickActionTypeNode},
    {"Win32 键盘控制类型枚举", Spawn_EnumKeyContorlTypeNode},
    {"Win32 键盘按键动作枚举", Spawn_EnumKeyClickActionTypeNode},
    {"Win32 软件鼠标点击", Spawn_Win32_SoftInput_MouseClick},
    {"Win32 软件鼠标移动", Spawn_Win32_SoftInput_MouseMove},
    {"Win32 软件鼠标滚轮", Spawn_Win32_SoftInput_MouseWheel},
    {"Win32 软件键盘点击", Spawn_Win32_SoftInput_KeyClick},
    {"Win32 后台鼠标点击", Spawn_Win32_PostMessage_MouseClick},
    {"Win32 后台鼠标移动", Spawn_Win32_PostMessage_MouseMove},
    {"Win32 后台鼠标滚轮", Spawn_Win32_PostMessage_MouseWheel},
    {"Win32 后台键盘点击", Spawn_Win32_PostMessage_KeyClick},
};
