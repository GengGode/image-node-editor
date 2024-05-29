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

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int threshold = 128;
        get_value(graph, node->Inputs[1], threshold);

        int max_value = 255;
        get_value(graph, node->Inputs[2], max_value);

        int type = cv::THRESH_BINARY;
        get_value(graph, node->Inputs[3], type);

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

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int max_value = 255;
        get_value(graph, node->Inputs[1], max_value);

        int type = cv::ADAPTIVE_THRESH_MEAN_C;
        get_value(graph, node->Inputs[2], type);

        int block_size = 11;
        get_value(graph, node->Inputs[3], block_size);

        int c = 2;
        get_value(graph, node->Inputs[4], c);

        try_catch_block;
        cv::Mat thresholded;
        cv::adaptiveThreshold(image, thresholded, max_value, type, cv::THRESH_BINARY, block_size, c);
        node->Outputs[0].SetValue(thresholded);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 多通道图像阈值
// Image Channel Threshold
Node *Spawn_ImageOperator_ChannelThresholding(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "多通道图像阈值");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道1阈值", 128);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道1最大值", 128);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道1阈值类型", cv::THRESH_BINARY);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道2阈值", 128);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道2最大值", 128);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道2阈值类型", cv::THRESH_BINARY);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道3阈值", 128);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道3最大值", 128);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道3阈值类型", cv::THRESH_BINARY);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道4阈值", 128);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道4最大值", 128);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道4阈值类型", cv::THRESH_BINARY);

    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    for (auto &output : node.Outputs)
        output.app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int threshold1 = 128;
        get_value(graph, node->Inputs[1], threshold1);
        int max_value1 = 255;
        get_value(graph, node->Inputs[2], max_value1);
        int type1 = cv::THRESH_BINARY;
        get_value(graph, node->Inputs[3], type1);

        int threshold2 = 128;
        get_value(graph, node->Inputs[4], threshold2);
        int max_value2 = 255;
        get_value(graph, node->Inputs[5], max_value2);
        int type2 = cv::THRESH_BINARY;
        get_value(graph, node->Inputs[6], type2);

        int threshold3 = 128;
        get_value(graph, node->Inputs[7], threshold3);
        int max_value3 = 255;
        get_value(graph, node->Inputs[8], max_value3);
        int type3 = cv::THRESH_BINARY;
        get_value(graph, node->Inputs[9], type3);

        int threshold4 = 128;
        get_value(graph, node->Inputs[10], threshold4);
        int max_value4 = 255;
        get_value(graph, node->Inputs[11], max_value4);
        int type4 = cv::THRESH_BINARY;
        get_value(graph, node->Inputs[12], type4);

        try_catch_block
        {
            cv::Mat thresholded;
            std::vector<cv::Mat> channels;
            cv::split(image, channels);
            if (channels.size() > 0)
                cv::threshold(channels[0], channels[0], threshold1, max_value1, type1);
            if (channels.size() > 1)
                cv::threshold(channels[1], channels[1], threshold2, max_value2, type2);
            if (channels.size() > 2)
                cv::threshold(channels[2], channels[2], threshold3, max_value3, type3);
            if (channels.size() > 3)
                cv::threshold(channels[3], channels[3], threshold4, max_value4, type4);
            cv::merge(channels, thresholded);
            node->Outputs[0].SetValue(thresholded);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// image inrange
Node *Spawn_ImageOperator_InRange(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "彩图二值化");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Color);
    node.Inputs.emplace_back(GetNextId(), PinType::Color);
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        cv::Scalar lower_bound(0, 0, 0);
        get_value(graph, node->Inputs[1], lower_bound);

        cv::Scalar upper_bound(255, 255, 255);
        get_value(graph, node->Inputs[2], upper_bound);

        try_catch_block;
        cv::Mat thresholded;
        cv::inRange(image, lower_bound, upper_bound, thresholded);
        node->Outputs[0].SetValue(thresholded);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageOperatorThresholdNodes = {
    {"阈值", Spawn_ImageOperator_Threshold},
    {"高级阈值", Spawn_ImageOperator_AdaptiveThreshold},
    {"多通道图像阈值", Spawn_ImageOperator_ChannelThresholding},
    {"彩图二值化", Spawn_ImageOperator_InRange},
};