#pragma once
#include "base_nodes.hpp"
// #include <json.hpp>

#define auto_resize_outputs(output_count)                                               \
    if (output_count <= 0)                                                              \
        return ExecuteResult::ErrorNode(node->ID, "next Task 数量必须大于0");           \
    if (output_count < node->Outputs.size())                                            \
    {                                                                                   \
        node->Outputs.erase(node->Outputs.begin() + output_count, node->Outputs.end()); \
    }                                                                                   \
    if (output_count > node->Outputs.size())                                            \
    {                                                                                   \
        for (int i = 0; i < output_count; i++)                                          \
        {                                                                               \
            if (i < node->Outputs.size())                                               \
                continue;                                                               \
            std::string name = "next " + std::to_string(i + 1);                         \
            node->Outputs.emplace_back(graph->get_next_id(), PinType::Flow, name);      \
            node->Outputs[node->Outputs.size() - 1].app = graph->env.app;               \
        }                                                                               \
    }

struct maa_base_node_state_value : public node_state_value
{
    // json::value value;
};
// startup task node
Node *Spawn_Maa_StartupTask(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "maa 入口任务");
    auto &node = m_Nodes.back();
    node.Type = NodeType::MaaTaskFlow;

    node.Inputs.push_back(Pin(GetNextId(), PinType::String, "Task名称", std::string("Startup Task")));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Int, "next Task 数量", 1));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "next 1"));

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string task_name;
        get_value(graph, node->Inputs[0], task_name);

        int next_task_count;
        get_value(graph, node->Inputs[1], next_task_count);

        // Display image
        node->Inputs[0].Value = task_name;
        node->Inputs[1].Value = next_task_count;

        try_catch_block
        {
            if (node->state_value == nullptr)
                node->state_value = std::make_shared<maa_base_node_state_value>();
            auto &task_value = std::static_pointer_cast<maa_base_node_state_value>(node->state_value);
            if (task_value == nullptr)
                return ExecuteResult::ErrorNode(node->ID, "状态值类型错误");

            auto_resize_outputs(next_task_count);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static EnumType MaaTaskFlowActionType = EnumType{{{0, "DoNothing"}, {1, "Click"}, {2, "Swipe"}, {3, "Key"}, {4, "Text"}, {5, "StartApp"}, {6, "StopApp"}, {7, "StopTask"}, {8, "Custom"}}};
// enum output node
Node *Spawn_EnumOutputNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "maa 任务流动作枚举");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;

    node.Outputs.push_back(Pin(GetNextId(), PinType::Enum, "任务流动作", EnumValue{MaaTaskFlowActionType, 0}));

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        try_catch_block

            // node->Outputs[0].Value = 0;

            catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// ocr task node
Node *Spawn_Maa_OcrTask(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "maa Ocr 任务");
    auto &node = m_Nodes.back();
    node.Type = NodeType::MaaTaskFlow;
    node.Inputs.push_back(Pin(GetNextId(), PinType::Flow, "依赖"));

    node.Inputs.push_back(Pin(GetNextId(), PinType::String, "Task名称", std::string("Ocr Task")));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Rect, "ROI", cv::Rect(0, 0, 0, 0)));
    node.Inputs.push_back(Pin(GetNextId(), PinType::String, "期望的结果", std::string("")));
    node.Inputs.push_back(Pin(GetNextId(), PinType::String, "替换", std::string("")));
    node.Inputs.push_back(Pin(GetNextId(), PinType::String, "排序方式", std::string("Horizontal")));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Int, "命中第几个结果", 0));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Bool, "仅识别", false));
    node.Inputs.push_back(Pin(GetNextId(), PinType::String, "模型文件夹路径", std::string("")));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Enum, "动作", EnumValue{MaaTaskFlowActionType, 0}));

    node.Inputs.push_back(Pin(GetNextId(), PinType::Int, "next Task 数量", 1));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "next 1"));

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string task_name;
        get_value(graph, node->Inputs[1], task_name);
        cv::Rect roi;
        get_value(graph, node->Inputs[2], roi);
        std::string expect_result;
        get_value(graph, node->Inputs[3], expect_result);
        std::string replace;
        get_value(graph, node->Inputs[4], replace);
        std::string sort_way;
        get_value(graph, node->Inputs[5], sort_way);
        int hit_index;
        get_value(graph, node->Inputs[6], hit_index);
        bool only_recognize;
        get_value(graph, node->Inputs[7], only_recognize);
        std::string model_folder_path;
        get_value(graph, node->Inputs[8], model_folder_path);
        EnumValue action;
        get_value(graph, node->Inputs[9], action);

        int next_task_count;
        get_value(graph, node->Inputs[10], next_task_count);

        // Display image
        node->Inputs[1].Value = task_name;
        node->Inputs[2].Value = roi;
        node->Inputs[3].Value = expect_result;
        node->Inputs[4].Value = replace;
        node->Inputs[5].Value = sort_way;
        node->Inputs[6].Value = hit_index;
        node->Inputs[7].Value = only_recognize;
        node->Inputs[8].Value = model_folder_path;
        node->Inputs[9].Value = action;
        node->Inputs[10].Value = next_task_count;

        try_catch_block
        {
            if (node->state_value == nullptr)
                node->state_value = std::make_shared<maa_base_node_state_value>();
            auto &task_value = std::static_pointer_cast<maa_base_node_state_value>(node->state_value);
            if (task_value == nullptr)
                return ExecuteResult::ErrorNode(node->ID, "状态值类型错误");

            auto_resize_outputs(next_task_count);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

/*
recognition : string
识别算法类型。可选，默认 DirectHit。
可选的值：DirectHit | TemplateMatch | FeatureMatch | ColorMatch | OCR | NeuralNetworkClassify | NeuralNetworkDetect | Custom
详见 算法类型。
*/
/*
action: strings
执行的动作。可选，默认 DoNothing。
可选的值：DoNothing | Click | Swipe | Key | Text | StartApp | StopApp | StopTask | Custom
详见 动作类型。
*/
// DoNothing
// Click
// Swipe
// Key
// Text
// StartApp
// StopApp
// StopTask
// Custom

// DirectHit
// TemplateMatch
// FeatureMatch
// ColorMatch
// OCR
// NeuralNetworkClassify
// NeuralNetworkDetect
// Custom

static NodeWorldGlobal::FactoryGroupFunc_t MaaTaskFlowNodes = {
    {"maa 入口任务", Spawn_Maa_StartupTask},
    {"maa 任务流动作枚举", Spawn_EnumOutputNode},
    {"maa OCR 任务", Spawn_Maa_OcrTask},
};
