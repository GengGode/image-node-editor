#pragma once
#include "base_nodes.hpp"

#define auto_resize_inputs(input_count, input_name, count_begin_number)                                  \
    if (input_count <= 0)                                                                                \
        return ExecuteResult::ErrorNode(node->ID, std::string(input_name) + " 数量必须大于0");           \
    if (input_count < node->Inputs.size() - count_begin_number)                                          \
    {                                                                                                    \
        node->Inputs.erase(node->Inputs.begin() + input_count + count_begin_number, node->Inputs.end()); \
    }                                                                                                    \
    if (input_count > node->Inputs.size())                                                               \
    {                                                                                                    \
        for (int i = 0; i < input_count; i++)                                                            \
        {                                                                                                \
            if (i < node->Inputs.size() - count_begin_number)                                            \
                continue;                                                                                \
            std::string name = std::string(input_name) + " " + std::to_string(i + 1);                    \
            node->Inputs.emplace_back(graph->get_next_id(), PinType::Flow, name);                        \
            node->Inputs[node->Inputs.size() - 1].app = graph->env.app;                                  \
            node->Inputs[node->Inputs.size() - 1].Kind = PinKind::Input;                                 \
        }                                                                                                \
    }

#define auto_resize_outputs(output_count, output_name, count_begin_number)                                   \
    if (output_count <= 0)                                                                                   \
        return ExecuteResult::ErrorNode(node->ID, std::string(output_name) + " 数量必须大于0");              \
    if (output_count < node->Outputs.size() - count_begin_number)                                            \
    {                                                                                                        \
        node->Outputs.erase(node->Outputs.begin() + output_count + count_begin_number, node->Outputs.end()); \
    }                                                                                                        \
    if (output_count > node->Outputs.size())                                                                 \
    {                                                                                                        \
        for (int i = 0; i < output_count; i++)                                                               \
        {                                                                                                    \
            if (i < node->Outputs.size() - count_begin_number)                                               \
                continue;                                                                                    \
            std::string name = std::string(output_name) + " " + std::to_string(i + 1);                       \
            node->Outputs.emplace_back(graph->get_next_id(), PinType::Flow, name);                           \
            node->Outputs[node->Outputs.size() - 1].app = graph->env.app;                                    \
            node->Outputs[node->Outputs.size() - 1].Kind = PinKind::Output;                                  \
        }                                                                                                    \
    }
#define break_flow_pin() value_index++
#define auto_get_value(type, vairant_name) \
    type vairant_name;                     \
    get_value(graph, node->Inputs[value_index++], vairant_name)

// 控制流入口
Node *Spawn_Flow_Startup(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "控制流入口");
    auto &node = m_Nodes.back();
    node.Type = NodeType::FlowSource;

    node.Inputs.push_back(Pin(GetNextId(), PinType::Int, "并行任务", 1));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "任务 1"));

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value_index = 0;

        auto_get_value(int, par_task_number);

        try_catch_block
        {
            auto_resize_outputs(par_task_number, "任务", 0);
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// 控制流屏障 std::barrier
Node *Spawn_Flow_Shutdown(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "控制流屏障");
    auto &node = m_Nodes.back();
    node.Type = NodeType::FlowSource;

    node.Inputs.push_back(Pin(GetNextId(), PinType::Int, "屏障数量", 1));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Int, "并行任务", 1));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Flow, "屏障任务 1"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "任务 1"));

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value_index = 0;

        auto_get_value(int, in_barrier_number);
        auto_get_value(int, out_par_task_number);

        try_catch_block
        {
            auto_resize_inputs(in_barrier_number, "屏障任务", 2);
            auto_resize_outputs(out_par_task_number, "任务", 0);
            graph->build_node(node);
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// 控制流 if 分支
Node *Spawn_Flow_If(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "控制流 条件分支");
    auto &node = m_Nodes.back();
    node.Type = NodeType::FlowSource;

    node.Inputs.push_back(Pin(GetNextId(), PinType::Flow));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Bool, "条件", true));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "是"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "否"));

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value_index = 0;

        break_flow_pin();
        auto_get_value(bool, condition);

        try_catch_block
        {
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// 控制流 while 循环
Node *Spawn_Flow_While(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "控制流 条件循环");
    auto &node = m_Nodes.back();
    node.Type = NodeType::FlowSource;

    node.Inputs.push_back(Pin(GetNextId(), PinType::Flow));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Bool, "条件", false));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "循环体"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "完成"));

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value_index = 0;

        break_flow_pin();
        auto_get_value(bool, condition);

        try_catch_block
        {
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// for each loop
Node *Spawn_Flow_ForEach(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "控制流 遍历循环");
    auto &node = m_Nodes.back();
    node.Type = NodeType::FlowSource;

    node.Inputs.push_back(Pin(GetNextId(), PinType::Flow));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Array, "数组"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "循环体"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::ArrayElement, "元素"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Int, "索引"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "完成"));

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value_index = 0;

        break_flow_pin();
        auto_get_value(Array, array);

        try_catch_block
        {
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// for each loop with break
Node *Spawn_Flow_ForEachBreak(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "控制流 遍历循环(带中断)");
    auto &node = m_Nodes.back();
    node.Type = NodeType::FlowSource;

    node.Inputs.push_back(Pin(GetNextId(), PinType::Flow));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Array, "数组"));
    node.Inputs.push_back(Pin(GetNextId(), PinType::Flow, "中断循环"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "循环体"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::ArrayElement, "元素"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Int, "索引"));
    node.Outputs.push_back(Pin(GetNextId(), PinType::Flow, "完成"));

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value_index = 0;

        break_flow_pin();
        auto_get_value(Array, array);

        try_catch_block
        {
        }
        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t FlowSourceNodes = {
    {"控制流入口", Spawn_Flow_Startup},
    {"控制流屏障", Spawn_Flow_Shutdown},
    {"控制流 条件分支", Spawn_Flow_If},
    {"控制流 条件循环", Spawn_Flow_While},
    {"控制流 遍历循环", Spawn_Flow_ForEach},
    {"控制流 遍历循环(带中断)", Spawn_Flow_ForEachBreak},
};
static std::vector<std::pair<std::string, factory_func_t>> FlowSourceNodesFactorys = {
    {"控制流/源/入口", Spawn_Flow_Startup},
    {"控制流/源/屏障", Spawn_Flow_Shutdown},
    {"控制流/源/条件分支", Spawn_Flow_If},
    {"控制流/源/条件循环", Spawn_Flow_While},
    {"控制流/源/遍历循环", Spawn_Flow_ForEach},
    {"控制流/源/遍历循环(带中断)", Spawn_Flow_ForEachBreak},
};