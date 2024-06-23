#pragma once
#include "base_nodes.hpp"

// find window node
Node *Spawn_Win32_Window(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// enum window node
Node *Spawn_Win32_EnumWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// move window node
Node *Spawn_Win32_MoveWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// resize window node
Node *Spawn_Win32_ResizeWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// toggle window node
Node *Spawn_Win32_ToggleWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// close window node
Node *Spawn_Win32_CloseWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// get window text node
Node *Spawn_Win32_GetWindowText(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// set window text node
Node *Spawn_Win32_SetWindowText(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// get top window node
Node *Spawn_Win32_GetTopWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// get window rect node
Node *Spawn_Win32_GetWindowRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// get window client rect node
Node *Spawn_Win32_GetClientRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// get active window node
Node *Spawn_Win32_GetActiveWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// set active window node
Node *Spawn_Win32_SetActiveWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);

static NodeWorldGlobal::FactoryGroupFunc_t Win32WindowNodes = {
    {"Win32 窗口句柄", Spawn_Win32_Window},
    {"Win32 枚举窗口", Spawn_Win32_EnumWindow},
    {"Win32 移动窗口", Spawn_Win32_MoveWindow},
    {"Win32 调整窗口大小", Spawn_Win32_ResizeWindow},
    {"Win32 切换窗口", Spawn_Win32_ToggleWindow},
    {"Win32 关闭窗口", Spawn_Win32_CloseWindow},
    {"Win32 获取窗口文本", Spawn_Win32_GetWindowText},
    {"Win32 设置窗口文本", Spawn_Win32_SetWindowText},
    {"Win32 获取顶层窗口", Spawn_Win32_GetTopWindow},
    {"Win32 获取窗口矩形", Spawn_Win32_GetWindowRect},
    {"Win32 获取客户区矩形", Spawn_Win32_GetClientRect},
    {"Win32 获取活动窗口", Spawn_Win32_GetActiveWindow},
    {"Win32 设置活动窗口", Spawn_Win32_SetActiveWindow},
};

static std::vector<std::pair<std::string, factory_func_t>> Win32WindowNodesFactorys = {
    {"Win32/窗口句柄", Spawn_Win32_Window},
    {"Win32/枚举窗口", Spawn_Win32_EnumWindow},
    {"Win32/移动窗口", Spawn_Win32_MoveWindow},
    {"Win32/调整窗口大小", Spawn_Win32_ResizeWindow},
    {"Win32/切换窗口", Spawn_Win32_ToggleWindow},
    {"Win32/关闭窗口", Spawn_Win32_CloseWindow},
    {"Win32/获取/窗口文本", Spawn_Win32_GetWindowText},
    {"Win32/设置/窗口文本", Spawn_Win32_SetWindowText},
    {"Win32/获取/顶层窗口", Spawn_Win32_GetTopWindow},
    {"Win32/获取/窗口矩形", Spawn_Win32_GetWindowRect},
    {"Win32/获取/客户区矩形", Spawn_Win32_GetClientRect},
    {"Win32/获取/活动窗口", Spawn_Win32_GetActiveWindow},
    {"Win32/设置/活动窗口", Spawn_Win32_SetActiveWindow},
};