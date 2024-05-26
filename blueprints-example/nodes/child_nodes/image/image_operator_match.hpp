#pragma once
#include "base_nodes.hpp"

static EnumType ImageTemplateMatchingMethodEnum = {
    {0, "TM_SQDIFF"},
    {1, "TM_SQDIFF_NORMED"},
    {2, "TM_CCORR"},
    {3, "TM_CCORR_NORMED"},
    {4, "TM_CCOEFF"},
    {5, "TM_CCOEFF_NORMED"},
};
// image template matching method node
Node *Spawn_ImageOperator_TemplateMatchingMethodEnum(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "模板匹配方法枚举");
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Outputs.emplace_back(GetNextId(), PinType::Enum, "匹配方法", EnumValue{ImageTemplateMatchingMethodEnum, 5});

    node.OnExecute = [](Graph *graph, Node *node)
    {
        try_catch_block {}
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// image template matching
Node *Spawn_ImageOperator_TemplateMatching(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "模板匹配");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Enum, "Method", EnumValue{ImageTemplateMatchingMethodEnum, 5});
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Mat templ;
        result = get_image(graph, node->Inputs[1], templ);
        if (result.has_error())
            return result;

        EnumValue method;
        result = get_value(graph, node->Inputs[2], method);

        node->Inputs[0].Value = image;
        node->Inputs[1].Value = templ;
        node->Inputs[2].Value = method;

        try_catch_block;
        cv::Mat result_image;
        cv::matchTemplate(image, templ, result_image, method.second);
        node->Outputs[0].SetValue(result_image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// get min-max pixel position
Node *Spawn_ImageOperator_MinMaxLoc(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取最值位置");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs.emplace_back(GetNextId(), PinType::Point, "最大值位置");
    node.Outputs.emplace_back(GetNextId(), PinType::Float, "最大值");
    node.Outputs.emplace_back(GetNextId(), PinType::Point, "最小值位置");
    node.Outputs.emplace_back(GetNextId(), PinType::Float, "最小值");

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        try_catch_block;
        double min_val, max_val;
        cv::Point min_loc, max_loc;
        cv::minMaxLoc(image, &min_val, &max_val, &min_loc, &max_loc);

        node->Outputs[0].SetValue(max_loc);
        node->Outputs[1].SetValue(static_cast<float>(max_val));
        node->Outputs[2].SetValue(min_loc);
        node->Outputs[3].SetValue(static_cast<float>(min_val));
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageOperatorMatchNodes = {
    {"模板匹配", Spawn_ImageOperator_TemplateMatching},
    {"获取最值位置", Spawn_ImageOperator_MinMaxLoc},
};