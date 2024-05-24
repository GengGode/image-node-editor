#include "win32_softinput.hpp"

static EnumType MouseClickType = {
    {0, "左键"},
    {1, "右键"},
    {2, "中键"},
};

// enum mouse click type node
Node *Spawn_EnumMouseClickTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 鼠标点击类型枚举");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;

    node.Outputs.push_back(Pin(GetNextId(), PinType::Enum, "鼠标按键", EnumValue{MouseClickType, 0}));

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        try_catch_block
            catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static EnumType MouseClickActionType = {
    {0, "按下"},
    {1, "释放"},
    {2, "单击"},
    {3, "双击"},
    {4, "长按"},
};

// enum mouse click action type node
Node *Spawn_EnumMouseClickActionTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 鼠标点击动作枚举");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;

    node.Outputs.push_back(Pin(GetNextId(), PinType::Enum, "鼠标动作", EnumValue{MouseClickActionType, 0}));

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        try_catch_block
            catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// software mouse click node
Node *Spawn_Win32_SoftInput_MouseClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 软件鼠标点击");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;
    node.Inputs.emplace_back(GetNextId(), PinType::Win32Handle, "窗口句柄");
    node.Inputs.emplace_back(GetNextId(), PinType::Point);
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "按键", EnumValue{MouseClickType, 0});
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "动作", EnumValue{MouseClickActionType, 0});
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "等待毫秒", 0);
    node.Outputs.emplace_back(GetNextId(), PinType::Bool, "状态");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "错误码");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);
        cv::Point point;
        get_value(graph, node->Inputs[1], point);
        EnumValue key;
        get_value(graph, node->Inputs[2], key);
        EnumValue action;
        get_value(graph, node->Inputs[3], action);
        int wait_ms;
        get_value(graph, node->Inputs[4], wait_ms);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = point;
        node->Inputs[2].Value = key;
        node->Inputs[3].Value = action;
        node->Inputs[4].Value = wait_ms;

        try_catch_block
        {
            auto key_name = key.first[key.second];
            auto action_name = action.first[action.second];

            std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));

            INPUT input;
            input.type = INPUT_MOUSE;
            input.mi.dx = point.x;
            input.mi.dy = point.y;
            input.mi.mouseData = 0;
            input.mi.dwFlags = 0;
            if (key_name == "左键")
                input.mi.dwFlags = action_name == "按下" ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
            else if (key_name == "右键")
                input.mi.dwFlags = action_name == "按下" ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
            else if (key_name == "中键")
                input.mi.dwFlags = action_name == "按下" ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;

            if (action_name == "单击")
            {
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, &input, sizeof(INPUT));
            }
            else if (action_name == "双击")
            {
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, &input, sizeof(INPUT));
            }
            else if (action_name == "长按")
            {
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, &input, sizeof(INPUT));
            }
            else
            {
                auto res = SendMessageTimeoutW(handle, WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y), SMTO_NORMAL, 100, nullptr);
            }

            // node->Outputs[0].Value = res != 0;
            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);

    return &node;
}
// software mouse move node
Node *Spawn_Win32_SoftInput_MouseMove(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 软件鼠标移动");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;
    node.Inputs.emplace_back(GetNextId(), PinType::Win32Handle, "窗口句柄");
    node.Inputs.emplace_back(GetNextId(), PinType::Point);
    node.Inputs.emplace_back(GetNextId(), PinType::Bool, "是否相对坐标", false);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "等待毫秒", 0);
    node.Outputs.emplace_back(GetNextId(), PinType::Bool, "状态");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "错误码");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);
        cv::Point point;
        get_value(graph, node->Inputs[1], point);
        bool relative;
        get_value(graph, node->Inputs[2], relative);
        int wait_ms;
        get_value(graph, node->Inputs[3], wait_ms);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = point;
        node->Inputs[2].Value = relative;
        node->Inputs[3].Value = wait_ms;

        try_catch_block
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));

            if (relative)
            {
                POINT p;
                GetCursorPos(&p);
                point.x += p.x;
                point.y += p.y;
            }

            auto res = SendMessageTimeoutW(handle, WM_MOUSEMOVE, 0, MAKELPARAM(point.x, point.y), SMTO_NORMAL, 100, nullptr);

            node->Outputs[0].Value = res != 0;
            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);

    return &node;
}
// software mouse wheel node
Node *Spawn_Win32_SoftInput_MouseWheel(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 软件鼠标滚轮");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;
    node.Inputs.emplace_back(GetNextId(), PinType::Win32Handle, "窗口句柄");
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "滚动值");
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "等待毫秒", 0);
    node.Outputs.emplace_back(GetNextId(), PinType::Bool, "状态");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "错误码");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);
        int delta;
        get_value(graph, node->Inputs[1], delta);
        int wait_ms;
        get_value(graph, node->Inputs[2], wait_ms);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = delta;
        node->Inputs[2].Value = wait_ms;

        try_catch_block
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));

            auto res = SendMessageTimeoutW(handle, WM_MOUSEWHEEL, delta, 0, SMTO_NORMAL, 100, nullptr);

            node->Outputs[0].Value = res != 0;
            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);

    return &node;
}
// software key press node
Node *Spawn_Win32_SoftInput_KeyPress(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 软件键盘按下");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;
    node.Inputs.emplace_back(GetNextId(), PinType::Win32Handle, "窗口句柄");
    node.Inputs.emplace_back(GetNextId(), PinType::String, "按键");
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "等待毫秒", 0);
    node.Outputs.emplace_back(GetNextId(), PinType::Bool, "状态");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "错误码");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);
        std::string key;
        get_value(graph, node->Inputs[1], key);
        int wait_ms;
        get_value(graph, node->Inputs[2], wait_ms);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = key;
        node->Inputs[2].Value = wait_ms;

        try_catch_block
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));

            INPUT input;
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = VkKeyScanA(key[0]);
            input.ki.wScan = 0;
            input.ki.dwFlags = 0;
            input.ki.time = 0;
            input.ki.dwExtraInfo = 0;

            auto res = SendMessageTimeoutW(handle, WM_KEYDOWN, input.ki.wVk, 0, SMTO_NORMAL, 100, nullptr);

            node->Outputs[0].Value = res != 0;
            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);

    return &node;
}
// software key release node
Node *Spawn_Win32_SoftInput_KeyRelease(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 软件键盘释放");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;
    node.Inputs.emplace_back(GetNextId(), PinType::Win32Handle, "窗口句柄");
    node.Inputs.emplace_back(GetNextId(), PinType::String, "按键");
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "等待毫秒", 0);
    node.Outputs.emplace_back(GetNextId(), PinType::Bool, "状态");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "错误码");

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);
        std::string key;
        get_value(graph, node->Inputs[1], key);
        int wait_ms;
        get_value(graph, node->Inputs[2], wait_ms);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = key;
        node->Inputs[2].Value = wait_ms;

        try_catch_block
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));

            INPUT input;
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = VkKeyScanA(key[0]);
            input.ki.wScan = 0;
            input.ki.dwFlags = KEYEVENTF_KEYUP;
            input.ki.time = 0;
            input.ki.dwExtraInfo = 0;

            auto res = SendMessageTimeoutW(handle, WM_KEYUP, input.ki.wVk, 0, SMTO_NORMAL, 100, nullptr);

            node->Outputs[0].Value = res != 0;
            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);

    return &node;
}
