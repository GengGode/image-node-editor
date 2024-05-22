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
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

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

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = threshold1;
        node->Inputs[2].Value = max_value1;
        node->Inputs[3].Value = type1;
        node->Inputs[4].Value = threshold2;
        node->Inputs[5].Value = max_value2;
        node->Inputs[6].Value = type2;
        node->Inputs[7].Value = threshold3;
        node->Inputs[8].Value = max_value3;
        node->Inputs[9].Value = type3;
        node->Inputs[10].Value = threshold4;
        node->Inputs[11].Value = max_value4;
        node->Inputs[12].Value = type4;

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

static NodeWorldGlobal::FactoryGroupFunc_t ImageOperatorThresholdNodes = {
    {"阈值", Spawn_ImageOperator_Threshold},
    {"高级阈值", Spawn_ImageOperator_AdaptiveThreshold},
    {"多通道图像阈值", Spawn_ImageOperator_ChannelThresholding},
};