#include "base_nodes.hpp"
#include <algorithm>

void graph_algorithm::dfs(Node *node, std::map<Node *, int> &indegree, std::vector<Node *> &stack, std::map<Node *, bool> &visited, std::vector<graph_algorithm::cycle> &cycles)
{
    if (visited[node])
        return; // 如果已访问，返回
    if (std::find(stack.begin(), stack.end(), node) != stack.end())
    {
        // 找到环，添加到cycles
        graph_algorithm::cycle cycle(stack.begin(), std::find(stack.begin(), stack.end(), node) + 1);
        cycles.push_back(cycle);
        return;
    }
    visited[node] = true;
    stack.push_back(node);
    // 对于每个邻接节点
    for (Link &link : graph->Links)
    {
        auto from_pin = graph->FindPin(link.StartPinID);
        if (from_pin == nullptr)
            continue;
        if (from_pin->Node != node)
            continue;
        auto to_pin = graph->FindPin(link.EndPinID);
        if (to_pin == nullptr)
            continue;
        if (to_pin->Node == nullptr)
            continue;
        dfs(to_pin->Node, indegree, stack, visited, cycles);
    }
    stack.pop_back();
}

std::vector<graph_algorithm::cycle> graph_algorithm::find_all_cycles()
{
    std::map<Node *, int> indegree;
    std::vector<graph_algorithm::cycle> cycles;
    std::vector<Node *> stack;
    std::map<Node *, bool> visited;

    // 初始化入度
    for (Link &link : graph->Links)
    {
        auto from_pin = graph->FindPin(link.StartPinID);
        if (from_pin == nullptr)
            continue;
        if (from_pin->Node == nullptr)
            continue;
        auto to_pin = graph->FindPin(link.EndPinID);
        if (to_pin == nullptr)
            continue;
        if (to_pin->Node == nullptr)
            continue;
        indegree[to_pin->Node]++;
    }

    // 对每个节点执行DFS
    for (Node &node : graph->Nodes)
    {
        dfs(&node, indegree, stack, visited, cycles);
    }

    // 根据入度排序环
    for (graph_algorithm::cycle &cycle : cycles)
    {
        std::sort(cycle.begin(), cycle.end(), [&indegree](Node *a, Node *b)
                  { return indegree[a] > indegree[b]; });
    }

    return cycles;
}