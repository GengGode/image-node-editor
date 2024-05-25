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

static EnumType KeyContorlType = {
    {0, "Null"},
    {1, "Ctrl"},
    {2, "Alt"},
    {3, "Shift"},
    {4, "Win"},
};
// software key contorl type node
Node *Spawn_EnumKeyContorlTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 键盘控制键枚举");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;

    node.Outputs.push_back(Pin(GetNextId(), PinType::Enum, "控制键", EnumValue{KeyContorlType, 0}));

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

static EnumType KeyClickActionType = {
    {0, "按下"},
    {1, "释放"},
    {2, "单击"},
};
// software key action type node
Node *Spawn_EnumKeyClickActionTypeNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 键盘按键动作枚举");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;

    node.Outputs.push_back(Pin(GetNextId(), PinType::Enum, "按键动作", EnumValue{KeyClickActionType, 0}));

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

#include <InputSimulator.hpp>

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

            WindowsInput::InputSimulator simulator;
            simulator.GetMouseSimulator().Sleep(wait_ms);

            simulator.GetMouseSimulator().MoveMouseTo(point.x, point.y);
            if (action_name == "按下")
            {
                if (key_name == "左键")
                    simulator.GetMouseSimulator().LeftButtonDown();
                else if (key_name == "右键")
                    simulator.GetMouseSimulator().RightButtonDown();
                else if (key_name == "中键")
                    simulator.GetMouseSimulator().MiddleButtonDown();
            }
            else if (action_name == "释放")
            {
                if (key_name == "左键")
                    simulator.GetMouseSimulator().LeftButtonUp();
                else if (key_name == "右键")
                    simulator.GetMouseSimulator().RightButtonUp();
                else if (key_name == "中键")
                    simulator.GetMouseSimulator().MiddleButtonUp();
            }
            else if (action_name == "单击")
            {
                if (key_name == "左键")
                    simulator.GetMouseSimulator().LeftButtonClick();
                else if (key_name == "右键")
                    simulator.GetMouseSimulator().RightButtonClick();
                else if (key_name == "中键")
                    simulator.GetMouseSimulator().MiddleButtonClick();
            }
            else if (action_name == "双击")
            {
                if (key_name == "左键")
                    simulator.GetMouseSimulator().LeftButtonDoubleClick();
                else if (key_name == "右键")
                    simulator.GetMouseSimulator().RightButtonDoubleClick();
                else if (key_name == "中键")
                    simulator.GetMouseSimulator().MiddleButtonDoubleClick();
            }
            else if (action_name == "长按")
            {
                if (key_name == "左键")
                    simulator.GetMouseSimulator().LeftButtonDown();
                else if (key_name == "右键")
                    simulator.GetMouseSimulator().RightButtonDown();
                else if (key_name == "中键")
                    simulator.GetMouseSimulator().MiddleButtonDown();
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
            WindowsInput::InputSimulator simulator;
            simulator.GetMouseSimulator().Sleep(wait_ms);

            if (relative)
                simulator.GetMouseSimulator().MoveMouseBy(point.x, point.y);
            else
                simulator.GetMouseSimulator().MoveMouseTo(point.x, point.y);

            // node->Outputs[0].Value = res != 0;
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
    node.Inputs.emplace_back(GetNextId(), PinType::Bool, "是否横向滚动", false);
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
        bool horizontal;
        get_value(graph, node->Inputs[3], horizontal);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = delta;
        node->Inputs[2].Value = wait_ms;
        node->Inputs[3].Value = horizontal;

        try_catch_block
        {
            WindowsInput::InputSimulator simulator;
            simulator.GetMouseSimulator().Sleep(wait_ms);

            if (horizontal)
                simulator.GetMouseSimulator().HorizontalScroll(delta);
            else
                simulator.GetMouseSimulator().VerticalScroll(delta);

            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);

    return &node;
}
// software key click node
Node *Spawn_Win32_SoftInput_KeyClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 软件键盘按键");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;
    node.Inputs.emplace_back(GetNextId(), PinType::Win32Handle, "窗口句柄");
    node.Inputs.emplace_back(GetNextId(), PinType::String, "按键", std::string("A"));
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "动作", EnumValue{KeyClickActionType, 2});
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "控制键1", EnumValue{KeyContorlType, 0});
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "控制键2", EnumValue{KeyContorlType, 0});
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "等待毫秒", 0);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);
        std::string key;
        get_value(graph, node->Inputs[1], key);
        EnumValue action;
        get_value(graph, node->Inputs[2], action);
        EnumValue control1;
        get_value(graph, node->Inputs[3], control1);
        EnumValue control2;
        get_value(graph, node->Inputs[4], control2);
        int wait_ms;
        get_value(graph, node->Inputs[5], wait_ms);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = key;
        node->Inputs[2].Value = action;
        node->Inputs[3].Value = control1;
        node->Inputs[4].Value = control2;
        node->Inputs[5].Value = wait_ms;

        try_catch_block
        {
            WindowsInput::InputSimulator simulator;
            simulator.GetKeyboardSimulator().Sleep(wait_ms);

            auto key_name = (char)key[0];
            auto action_name = action.first[action.second];
            auto control1_name = control1.first[control1.second];
            auto control2_name = control2.first[control2.second];

            if (control1_name == "Ctrl")
                simulator.GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::CONTROL_);
            if (control1_name == "Alt")
                simulator.GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::MENU);
            if (control1_name == "Shift")
                simulator.GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::SHIFT);
            if (control1_name == "Win")
                simulator.GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::LWIN);
            if (control2_name == "Ctrl")
                simulator.GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::CONTROL_);
            if (control2_name == "Alt")
                simulator.GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::MENU);
            if (control2_name == "Shift")
                simulator.GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::SHIFT);
            if (control2_name == "Win")
                simulator.GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::LWIN);

            if (action_name == "按下")
                simulator.GetKeyboardSimulator().KeyDown((WindowsInput::VirtualKeyCode)key_name);
            else if (action_name == "释放")
                simulator.GetKeyboardSimulator().KeyUp((WindowsInput::VirtualKeyCode)key_name);
            else if (action_name == "单击")
                simulator.GetKeyboardSimulator().KeyPress((WindowsInput::VirtualKeyCode)key_name);

            if (control1_name == "Ctrl")
                simulator.GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::CONTROL_);
            if (control1_name == "Alt")
                simulator.GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::MENU);
            if (control1_name == "Shift")
                simulator.GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::SHIFT);
            if (control1_name == "Win")
                simulator.GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::LWIN);
            if (control2_name == "Ctrl")
                simulator.GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::CONTROL_);
            if (control2_name == "Alt")
                simulator.GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::MENU);
            if (control2_name == "Shift")
                simulator.GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::SHIFT);
            if (control2_name == "Win")
                simulator.GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::LWIN);
        }
        catch_block_and_return;
    };
    BuildNode(&node);

    return &node;
}

#include <WindowsInputPostMessageDispatcher.hpp>
// post message mouse click node
Node *Spawn_Win32_PostMessage_MouseClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 后台鼠标点击");
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

            WindowsInput::InputSimulator *simulator = NULL;
            WindowsInput::WindowsInputPostMessageDispatcher dispatcher;
            WindowsInput::KeyboardSimulator *keyboardSimulator = new WindowsInput::KeyboardSimulator(simulator, &dispatcher);
            WindowsInput::MouseSimulator *mouseSimulator = new WindowsInput::MouseSimulator(simulator, &dispatcher);
            WindowsInput::WindowsInputDeviceStateAdaptor *deviceStateAdaptor = new WindowsInput::WindowsInputDeviceStateAdaptor();
            simulator = new WindowsInput::InputSimulator(keyboardSimulator, mouseSimulator, deviceStateAdaptor);

            dispatcher.targetWindow = handle;

            simulator->GetMouseSimulator().MoveMouseTo(point.x, point.y);
            if (action_name == "按下")
            {
                if (key_name == "左键")
                    simulator->GetMouseSimulator().LeftButtonDown();
                else if (key_name == "右键")
                    simulator->GetMouseSimulator().RightButtonDown();
                else if (key_name == "中键")
                    simulator->GetMouseSimulator().MiddleButtonDown();
            }
            else if (action_name == "释放")
            {
                if (key_name == "左键")
                    simulator->GetMouseSimulator().LeftButtonUp();
                else if (key_name == "右键")
                    simulator->GetMouseSimulator().RightButtonUp();
                else if (key_name == "中键")
                    simulator->GetMouseSimulator().MiddleButtonUp();
            }
            else if (action_name == "单击")
            {
                if (key_name == "左键")
                    simulator->GetMouseSimulator().LeftButtonClick();
                else if (key_name == "右键")
                    simulator->GetMouseSimulator().RightButtonClick();
                else if (key_name == "中键")
                    simulator->GetMouseSimulator().MiddleButtonClick();
            }
            else if (action_name == "双击")
            {
                if (key_name == "左键")
                    simulator->GetMouseSimulator().LeftButtonDoubleClick();
                else if (key_name == "右键")
                    simulator->GetMouseSimulator().RightButtonDoubleClick();
                else if (key_name == "中键")
                    simulator->GetMouseSimulator().MiddleButtonDoubleClick();
            }
            else if (action_name == "长按")
            {
                if (key_name == "左键")
                    simulator->GetMouseSimulator().LeftButtonDown();
                else if (key_name == "右键")
                    simulator->GetMouseSimulator().RightButtonDown();
                else if (key_name == "中键")
                    simulator->GetMouseSimulator().MiddleButtonDown();
            }

            delete simulator;

            // node->Outputs[0].Value = res != 0;
            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}
// post message mouse move node
Node *Spawn_Win32_PostMessage_MouseMove(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 后台鼠标移动");
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
            WindowsInput::InputSimulator *simulator = NULL;
            WindowsInput::WindowsInputPostMessageDispatcher dispatcher;
            WindowsInput::KeyboardSimulator *keyboardSimulator = new WindowsInput::KeyboardSimulator(simulator, &dispatcher);
            WindowsInput::MouseSimulator *mouseSimulator = new WindowsInput::MouseSimulator(simulator, &dispatcher);
            WindowsInput::WindowsInputDeviceStateAdaptor *deviceStateAdaptor = new WindowsInput::WindowsInputDeviceStateAdaptor();
            simulator = new WindowsInput::InputSimulator(keyboardSimulator, mouseSimulator, deviceStateAdaptor);

            dispatcher.targetWindow = handle;

            if (relative)
                simulator->GetMouseSimulator().MoveMouseBy(point.x, point.y);
            else
                simulator->GetMouseSimulator().MoveMouseTo(point.x, point.y);

            delete simulator;

            // node->Outputs[0].Value = res != 0;
            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}
// post message mouse wheel node
Node *Spawn_Win32_PostMessage_MouseWheel(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 后台鼠标滚轮");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;
    node.Inputs.emplace_back(GetNextId(), PinType::Win32Handle, "窗口句柄");
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "滚动值");
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "等待毫秒", 0);
    node.Inputs.emplace_back(GetNextId(), PinType::Bool, "是否横向滚动", false);
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
        bool horizontal;
        get_value(graph, node->Inputs[3], horizontal);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = delta;
        node->Inputs[2].Value = wait_ms;
        node->Inputs[3].Value = horizontal;

        try_catch_block
        {
            WindowsInput::InputSimulator *simulator = NULL;
            WindowsInput::WindowsInputPostMessageDispatcher dispatcher;
            WindowsInput::KeyboardSimulator *keyboardSimulator = new WindowsInput::KeyboardSimulator(simulator, &dispatcher);
            WindowsInput::MouseSimulator *mouseSimulator = new WindowsInput::MouseSimulator(simulator, &dispatcher);
            WindowsInput::WindowsInputDeviceStateAdaptor *deviceStateAdaptor = new WindowsInput::WindowsInputDeviceStateAdaptor();
            simulator = new WindowsInput::InputSimulator(keyboardSimulator, mouseSimulator, deviceStateAdaptor);

            dispatcher.targetWindow = handle;

            if (horizontal)
                simulator->GetMouseSimulator().HorizontalScroll(delta);
            else
                simulator->GetMouseSimulator().VerticalScroll(delta);

            delete simulator;

            // node->Outputs[0].Value = res != 0;
            node->Outputs[1].Value = (int)GetLastError();
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}
// post message key action node
Node *Spawn_Win32_PostMessage_KeyClick(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Win32 后台键盘点击");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Win32;
    node.Inputs.emplace_back(GetNextId(), PinType::Win32Handle, "窗口句柄");
    node.Inputs.emplace_back(GetNextId(), PinType::String, "按键", std::string("A"));
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "动作", EnumValue{KeyClickActionType, 0});
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "控制键1", EnumValue{KeyContorlType, 0});
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "控制键2", EnumValue{KeyContorlType, 0});
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "等待毫秒", 0);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        HWND handle;
        get_value(graph, node->Inputs[0], handle);
        std::string key;
        get_value(graph, node->Inputs[1], key);
        EnumValue action;
        get_value(graph, node->Inputs[2], action);
        EnumValue control1;
        get_value(graph, node->Inputs[3], control1);
        EnumValue control2;
        get_value(graph, node->Inputs[4], control2);
        int wait_ms;
        get_value(graph, node->Inputs[5], wait_ms);

        // Display image
        node->Inputs[0].Value = handle;
        node->Inputs[1].Value = key;
        node->Inputs[2].Value = action;
        node->Inputs[3].Value = control1;
        node->Inputs[4].Value = control2;
        node->Inputs[5].Value = wait_ms;

        try_catch_block
        {
            WindowsInput::InputSimulator *simulator = NULL;
            WindowsInput::WindowsInputPostMessageDispatcher dispatcher;
            WindowsInput::KeyboardSimulator *keyboardSimulator = new WindowsInput::KeyboardSimulator(simulator, &dispatcher);
            WindowsInput::MouseSimulator *mouseSimulator = new WindowsInput::MouseSimulator(simulator, &dispatcher);
            WindowsInput::WindowsInputDeviceStateAdaptor *deviceStateAdaptor = new WindowsInput::WindowsInputDeviceStateAdaptor();
            simulator = new WindowsInput::InputSimulator(keyboardSimulator, mouseSimulator, deviceStateAdaptor);

            dispatcher.targetWindow = handle;

            auto key_name = (char)key[0];
            auto action_name = action.first[action.second];
            auto control1_name = control1.first[control1.second];
            auto control2_name = control2.first[control2.second];

            if (control1_name == "Ctrl")
                simulator->GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::CONTROL_);
            if (control1_name == "Alt")
                simulator->GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::MENU);
            if (control1_name == "Shift")
                simulator->GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::SHIFT);
            if (control1_name == "Win")
                simulator->GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::LWIN);
            if (control2_name == "Ctrl")
                simulator->GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::CONTROL_);
            if (control2_name == "Alt")
                simulator->GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::MENU);
            if (control2_name == "Shift")
                simulator->GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::SHIFT);
            if (control2_name == "Win")
                simulator->GetKeyboardSimulator().KeyDown(WindowsInput::VirtualKeyCode::LWIN);

            if (action_name == "按下")
                simulator->GetKeyboardSimulator().KeyDown((WindowsInput::VirtualKeyCode)key_name);
            else if (action_name == "释放")
                simulator->GetKeyboardSimulator().KeyUp((WindowsInput::VirtualKeyCode)key_name);
            else if (action_name == "单击")
                simulator->GetKeyboardSimulator().KeyPress((WindowsInput::VirtualKeyCode)key_name);

            if (control1_name == "Ctrl")
                simulator->GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::CONTROL_);
            if (control1_name == "Alt")
                simulator->GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::MENU);
            if (control1_name == "Shift")
                simulator->GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::SHIFT);
            if (control1_name == "Win")
                simulator->GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::LWIN);
            if (control2_name == "Ctrl")
                simulator->GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::CONTROL_);
            if (control2_name == "Alt")
                simulator->GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::MENU);
            if (control2_name == "Shift")
                simulator->GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::SHIFT);
            if (control2_name == "Win")
                simulator->GetKeyboardSimulator().KeyUp(WindowsInput::VirtualKeyCode::LWIN);

            delete simulator;
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}
