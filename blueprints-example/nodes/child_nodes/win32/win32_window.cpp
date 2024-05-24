#include "win32_window.hpp"

#include "utils.string.h"
#include "convert.string.h"

// find window node
Node *Spawn_Win32_Window(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 窗口句柄");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口名称", PinType::String, std::string());
    node.Inputs.emplace_back(GetNextId(), "窗口类名", PinType::String, std::string());

    node.Outputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle);
    node.Outputs.emplace_back(GetNextId(), PinType::Size, "窗口大小");
    node.Outputs.emplace_back(GetNextId(), PinType::Point, "窗口位置");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string window_name;
        get_value(graph, node->Inputs[0], window_name);

        std::string class_name;
        get_value(graph, node->Inputs[1], class_name);

        // Display image
        node->Inputs[0].Value = window_name;
        node->Inputs[1].Value = class_name;

        try_catch_block
        {

            auto w_class_name = utils::to_wstring(class_name);
            auto w_window_name = utils::to_wstring(window_name);
            const wchar_t *w_window_name_str = w_window_name.size() > 0 ? w_window_name.c_str() : nullptr;
            const wchar_t *w_class_name_str = w_class_name.size() > 0 ? w_class_name.c_str() : nullptr;

            cv::Mat image;

            HWND handle = FindWindowW(w_class_name_str, w_window_name_str);
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "未找到窗口");

            RECT rect;
            GetWindowRect(handle, &rect);
            cv::Point point = {rect.left, rect.top};
            cv::Size size = {rect.right - rect.left, rect.bottom - rect.top};

            node->Outputs[0].Value = handle;
            node->Outputs[1].Value = size;
            node->Outputs[2].Value = point;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// move window node
Node *Spawn_Win32_MoveWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 移动窗口");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);
    node.Inputs.emplace_back(GetNextId(), "窗口位置", PinType::Point, cv::Point(0, 0));

    node.Outputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle);
    node.Outputs.emplace_back(GetNextId(), PinType::Point, "窗口位置");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        cv::Point point;
        get_value(graph, node->Inputs[1], point);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = point;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            SetWindowPos(handle, NULL, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

            RECT rect;
            GetWindowRect(handle, &rect);
            cv::Point new_point = {rect.left, rect.top};

            node->Outputs[0].Value = handle;
            node->Outputs[1].Value = new_point;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// resize window node
Node *Spawn_Win32_ResizeWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 调整窗口大小");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);
    node.Inputs.emplace_back(GetNextId(), "窗口大小", PinType::Size, cv::Size(0, 0));

    node.Outputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle);
    node.Outputs.emplace_back(GetNextId(), PinType::Size, "窗口大小");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        cv::Size size;
        get_value(graph, node->Inputs[1], size);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = size;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            SetWindowPos(handle, NULL, 0, 0, size.width, size.height, SWP_NOMOVE | SWP_NOZORDER);

            RECT rect;
            GetWindowRect(handle, &rect);
            cv::Size new_size = {rect.right - rect.left, rect.bottom - rect.top};

            node->Outputs[0].Value = handle;
            node->Outputs[1].Value = new_size;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// toggle window node
Node *Spawn_Win32_ToggleWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 切换窗口");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);
    node.Inputs.emplace_back(GetNextId(), "是否显示", PinType::Bool, true);

    node.Outputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle);
    node.Outputs.emplace_back(GetNextId(), "是否显示", PinType::Bool);

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        bool is_show;
        get_value(graph, node->Inputs[1], is_show);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = is_show;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            ShowWindow(handle, is_show ? SW_SHOW : SW_HIDE);

            node->Outputs[0].Value = handle;
            node->Outputs[1].Value = is_show;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// close window node
Node *Spawn_Win32_CloseWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 关闭窗口");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        // Display image
        node->Inputs[0].Value = handle;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            SendMessage(handle, WM_CLOSE, 0, 0);

            node->Outputs[0].Value = handle;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// get window text node
Node *Spawn_Win32_GetWindowText(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 获取窗口文本");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);

    node.Outputs.emplace_back(GetNextId(), "窗口文本", PinType::String);

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        // Display image
        node->Inputs[0].Value = handle;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            int length = GetWindowTextLength(handle);
            if (length == 0)
                return ExecuteResult::ErrorNode(node->ID, "窗口文本为空");

            std::vector<wchar_t> buffer(length + 1);
            GetWindowTextW(handle, buffer.data(), length + 1);

            std::string text = utils::to_string(buffer.data());
            node->Outputs[0].Value = text;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// set window text node
Node *Spawn_Win32_SetWindowText(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 设置窗口文本");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);
    node.Inputs.emplace_back(GetNextId(), "窗口文本", PinType::String, std::string());

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        std::string text;
        get_value(graph, node->Inputs[1], text);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = text;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            auto w_text = utils::to_wstring(text);
            SetWindowTextW(handle, w_text.c_str());
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// get top window node
Node *Spawn_Win32_GetTopWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 获取顶层窗口");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Outputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle);

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        // Display image

        try_catch_block
        {
            HWND handle = GetForegroundWindow();
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "未找到顶层窗口");

            node->Outputs[0].Value = handle;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// get window rect node
Node *Spawn_Win32_GetWindowRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 获取窗口矩形");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);

    node.Outputs.emplace_back(GetNextId(), PinType::Rect, "窗口矩形");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        // Display image
        node->Inputs[0].Value = handle;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            RECT rect;
            GetWindowRect(handle, &rect);

            cv::Rect window_rect = {rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top};
            node->Outputs[0].Value = window_rect;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// get window client rect node
Node *Spawn_Win32_GetClientRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 获取客户区矩形");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);

    node.Outputs.emplace_back(GetNextId(), PinType::Rect, "客户区矩形");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        // Display image
        node->Inputs[0].Value = handle;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            RECT rect;
            GetClientRect(handle, &rect);

            cv::Rect client_rect = {rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top};
            node->Outputs[0].Value = client_rect;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// get active window node
Node *Spawn_Win32_GetActiveWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 获取活动窗口");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Outputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle);

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        // Display image

        try_catch_block
        {
            HWND handle = GetActiveWindow();
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "未找到活动窗口");

            node->Outputs[0].Value = handle;
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// set active window node
Node *Spawn_Win32_SetActiveWindow(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 设置活动窗口");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;

    node.Inputs.emplace_back(GetNextId(), "窗口句柄", PinType::Win32Handle, nullptr);

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);

        // Display image
        node->Inputs[0].Value = handle;

        try_catch_block
        {
            if (handle == NULL)
                return ExecuteResult::ErrorNode(node->ID, "窗口句柄为空");

            SetActiveWindow(handle);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
