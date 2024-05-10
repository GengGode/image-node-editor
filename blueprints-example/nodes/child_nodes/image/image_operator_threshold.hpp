#pragma once
#include "base_nodes.hpp"

// threshold
Node *Spawn_ImageOperator_Threshold(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "阈值");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOperation_Threshold;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Threshold", PinType::Int, 128);
    node.Inputs.emplace_back(GetNextId(), "Max Value", PinType::Int, 255);
    node.Inputs.emplace_back(GetNextId(), "Type", PinType::Int, cv::THRESH_BINARY);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int threshold = 128;
        result = get_value(graph, node->Inputs[1], threshold);
        if (result.has_error())
            return result;

        int max_value = 255;
        result = get_value(graph, node->Inputs[2], max_value);
        if (result.has_error())
            return result;

        int type = cv::THRESH_BINARY;
        result = get_value(graph, node->Inputs[3], type);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = image;
        node->Inputs[1].Value = threshold;
        node->Inputs[2].Value = max_value;
        node->Inputs[3].Value = type;

        try_catch_block;
        cv::Mat thresholded;
        cv::threshold(image, thresholded, threshold, max_value, type);
        node->Outputs[0].SetValue(thresholded);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// adaptive threshold
Node *Spawn_ImageOperator_AdaptiveThreshold(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "高级阈值");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOperation_Threshold;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Max Value", PinType::Int, 255);
    node.Inputs.emplace_back(GetNextId(), "Type", PinType::Int, cv::ADAPTIVE_THRESH_MEAN_C);
    node.Inputs.emplace_back(GetNextId(), "Block Size", PinType::Int, 11);
    node.Inputs.emplace_back(GetNextId(), "C", PinType::Int, 2);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int max_value = 255;
        result = get_value(graph, node->Inputs[1], max_value);
        if (result.has_error())
            return result;

        int type = cv::ADAPTIVE_THRESH_MEAN_C;
        result = get_value(graph, node->Inputs[2], type);
        if (result.has_error())
            return result;

        int block_size = 11;
        result = get_value(graph, node->Inputs[3], block_size);
        if (result.has_error())
            return result;

        int c = 2;
        result = get_value(graph, node->Inputs[4], c);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = image;
        node->Inputs[1].Value = max_value;
        node->Inputs[2].Value = type;
        node->Inputs[3].Value = block_size;
        node->Inputs[4].Value = c;

        try_catch_block;
        cv::Mat thresholded;
        cv::adaptiveThreshold(image, thresholded, max_value, type, cv::THRESH_BINARY, block_size, c);
        node->Outputs[0].SetValue(thresholded);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageOperatorThresholdNodes = {
    {"阈值", Spawn_ImageOperator_Threshold},
    {"高级阈值", Spawn_ImageOperator_AdaptiveThreshold},
};