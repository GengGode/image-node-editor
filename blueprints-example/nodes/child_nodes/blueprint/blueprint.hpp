#ifndef BLUEPRINT_HPP
#define BLUEPRINT_HPP

#include "base_nodes.hpp"

Node *SpawnInputActionNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "InputAction Fire", ImColor(255, 128, 128));
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Delegate);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Pressed", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Released", PinType::Flow);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnBranchNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Branch");
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "False", PinType::Flow);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnDoNNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Do N");
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Enter", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "N", PinType::Int);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Reset", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Exit", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Counter", PinType::Int);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnOutputActionNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "OutputAction");
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Sample", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Event", PinType::Delegate);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnPrintStringNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Print String");
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "In String", PinType::String);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

    BuildNode(&m_Nodes.back());
    return &m_Nodes.back();
}

Node *SpawnMessageNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "", ImColor(128, 195, 248));
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Outputs.emplace_back(GetNextId(), "Message", PinType::String, std::string("Hello, World!"));

    BuildNode(&node);

    return &node;
}

Node *SpawnSetTimerNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Set Timer", ImColor(128, 195, 248));
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Function Name", PinType::Function);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Time", PinType::Float);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Looping", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnLessNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "<", ImColor(128, 195, 248));
    m_Nodes.back().Type = NodeType::Simple;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnWeirdNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "o.O", ImColor(128, 195, 248));
    m_Nodes.back().Type = NodeType::Simple;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnTraceByChannelNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Single Line Trace by Channel", ImColor(255, 128, 64));
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Start", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "End", PinType::Int);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Channel", PinType::Float);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Complex", PinType::Bool);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Actors to Ignore", PinType::Int);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Draw Debug Type", PinType::Bool);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Ignore Self", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Out Hit", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Return Value", PinType::Bool);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnComment(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Test Comment");
    m_Nodes.back().Type = NodeType::Comment;
    m_Nodes.back().Size = ImVec2(300, 200);

    return &m_Nodes.back();
}

static NodeWorldGlobal::FactoryGroupFunc_t BlueprintNodes = {{"InputAction", SpawnInputActionNode},
                                                             {"Branch", SpawnBranchNode},
                                                             {"Do N", SpawnDoNNode},
                                                             {"OutputAction", SpawnOutputActionNode},
                                                             {"Print String", SpawnPrintStringNode},
                                                             {"Message", SpawnMessageNode},
                                                             {"Set Timer", SpawnSetTimerNode},
                                                             {"Less", SpawnLessNode},
                                                             {"Weird", SpawnWeirdNode},
                                                             {"Trace by Channel", SpawnTraceByChannelNode}};
static std::vector<std::pair<std::string, factory_func_t>> BlueprintNodesFactorys = {{"蓝图/InputAction", SpawnInputActionNode},
                                                                                     {"蓝图/Branch", SpawnBranchNode},
                                                                                     {"蓝图/Do N", SpawnDoNNode},
                                                                                     {"蓝图/OutputAction", SpawnOutputActionNode},
                                                                                     {"蓝图/Print String", SpawnPrintStringNode},
                                                                                     {"蓝图/Message", SpawnMessageNode},
                                                                                     {"蓝图/Set Timer", SpawnSetTimerNode},
                                                                                     {"蓝图/Less", SpawnLessNode},
                                                                                     {"蓝图/Weird", SpawnWeirdNode},
                                                                                     {"蓝图/Trace by Channel", SpawnTraceByChannelNode}};
#endif // BLUEPRINT_HPP