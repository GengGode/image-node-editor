#pragma once
#include "base_nodes.hpp"

// draw line
Node *Spawn_ImageOperator_DrawLine(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "画线");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "起点", PinType::Point, cv::Point(0, 0));
    node.Inputs.emplace_back(GetNextId(), "终点", PinType::Point, cv::Point(1024, 1024));
    node.Inputs.emplace_back(GetNextId(), "颜色", PinType::Color, cv::Scalar(0, 0, 0, 0));
    node.Inputs.emplace_back(GetNextId(), "线宽", PinType::Int, 1);
    node.Inputs.emplace_back(GetNextId(), "抗锯齿类型", PinType::Int, cv::LINE_8);
    node.Inputs.emplace_back(GetNextId(), "坐标中小数点位数", PinType::Int, 0);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Point start(0, 0);
        get_value(graph, node->Inputs[1], start);

        cv::Point end(1024, 1024);
        get_value(graph, node->Inputs[2], end);

        cv::Scalar color(0, 0, 0, 0);
        get_value(graph, node->Inputs[3], color);

        int thickness = 1;
        get_value(graph, node->Inputs[4], thickness);

        int lineType = cv::LINE_8;
        get_value(graph, node->Inputs[5], lineType);

        int shift = 0;
        get_value(graph, node->Inputs[6], shift);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = start;
        node->Inputs[2].Value = end;
        node->Inputs[3].Value = color;
        node->Inputs[4].Value = thickness;
        node->Inputs[5].Value = lineType;
        node->Inputs[6].Value = shift;

        try_catch_block
        {
            cv::Mat result = image.clone();
            cv::line(result, start, end, color, thickness, lineType, shift);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// draw rectangle
Node *Spawn_ImageOperator_DrawRectangle(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "画矩形");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "矩形", PinType::Rect, cv::Rect(0, 0, 1024, 1024));
    node.Inputs.emplace_back(GetNextId(), "颜色", PinType::Color, cv::Scalar(0, 0, 0, 0));
    node.Inputs.emplace_back(GetNextId(), "线宽", PinType::Int, 1);
    node.Inputs.emplace_back(GetNextId(), "抗锯齿类型", PinType::Int, cv::LINE_8);
    node.Inputs.emplace_back(GetNextId(), "坐标中小数点位数", PinType::Int, 0);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Rect rect(0, 0, 1024, 1024);
        get_value(graph, node->Inputs[1], rect);

        cv::Scalar color(0, 0, 0, 0);
        get_value(graph, node->Inputs[2], color);

        int thickness = 1;
        get_value(graph, node->Inputs[3], thickness);

        int lineType = cv::LINE_8;
        get_value(graph, node->Inputs[4], lineType);

        int shift = 0;
        get_value(graph, node->Inputs[5], shift);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = rect;
        node->Inputs[2].Value = color;
        node->Inputs[3].Value = thickness;

        try_catch_block
        {
            cv::Mat result = image.clone();
            cv::rectangle(result, rect, color, thickness, lineType, shift);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// draw circle
Node *Spawn_ImageOperator_DrawCircle(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "画圆");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "圆心", PinType::Point, cv::Point(512, 512));
    node.Inputs.emplace_back(GetNextId(), "半径", PinType::Int, 512);
    node.Inputs.emplace_back(GetNextId(), "颜色", PinType::Color, cv::Scalar(0, 0, 0, 0));
    node.Inputs.emplace_back(GetNextId(), "线宽", PinType::Int, 1);
    node.Inputs.emplace_back(GetNextId(), "抗锯齿类型", PinType::Int, cv::LINE_8);
    node.Inputs.emplace_back(GetNextId(), "坐标中小数点位数", PinType::Int, 0);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Point center(512, 512);
        get_value(graph, node->Inputs[1], center);

        int radius = 512;
        get_value(graph, node->Inputs[2], radius);

        cv::Scalar color(0, 0, 0, 0);
        get_value(graph, node->Inputs[3], color);

        int thickness = 1;
        get_value(graph, node->Inputs[4], thickness);

        int lineType = cv::LINE_8;
        get_value(graph, node->Inputs[5], lineType);

        int shift = 0;
        get_value(graph, node->Inputs[6], shift);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = center;
        node->Inputs[2].Value = radius;
        node->Inputs[3].Value = color;
        node->Inputs[4].Value = thickness;

        try_catch_block
        {
            cv::Mat result = image.clone();
            cv::circle(result, center, radius, color, thickness, lineType, shift);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// draw ellipse
Node *Spawn_ImageOperator_DrawEllipse(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "画椭圆");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "中心", PinType::Point, cv::Point(512, 512));
    node.Inputs.emplace_back(GetNextId(), "轴长", PinType::Size, cv::Size(512, 256));
    node.Inputs.emplace_back(GetNextId(), "旋转角度", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "起始角度", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "结束角度", PinType::Int, 360);
    node.Inputs.emplace_back(GetNextId(), "颜色", PinType::Color, cv::Scalar(0, 0, 0, 0));
    node.Inputs.emplace_back(GetNextId(), "线宽", PinType::Int, 1);
    node.Inputs.emplace_back(GetNextId(), "抗锯齿类型", PinType::Int, cv::LINE_8);
    node.Inputs.emplace_back(GetNextId(), "坐标中小数点位数", PinType::Int, 0);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Point center(512, 512);
        get_value(graph, node->Inputs[1], center);

        cv::Size axes(512, 256);
        get_value(graph, node->Inputs[2], axes);

        int angle = 0;
        get_value(graph, node->Inputs[3], angle);

        int startAngle = 0;
        get_value(graph, node->Inputs[4], startAngle);

        int endAngle = 360;
        get_value(graph, node->Inputs[5], endAngle);

        cv::Scalar color(0, 0, 0, 0);
        get_value(graph, node->Inputs[6], color);

        int thickness = 1;
        get_value(graph, node->Inputs[7], thickness);

        int lineType = cv::LINE_8;
        get_value(graph, node->Inputs[8], lineType);

        int shift = 0;
        get_value(graph, node->Inputs[9], shift);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = center;
        node->Inputs[2].Value = axes;
        node->Inputs[3].Value = angle;
        node->Inputs[4].Value = startAngle;
        node->Inputs[5].Value = endAngle;
        node->Inputs[6].Value = color;
        node->Inputs[7].Value = thickness;
        node->Inputs[8].Value = lineType;
        node->Inputs[9].Value = shift;

        try_catch_block
        {
            cv::Mat result = image.clone();
            cv::ellipse(result, center, axes, angle, startAngle, endAngle, color, thickness, lineType, shift);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// draw text
Node *Spawn_ImageOperator_DrawText(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "画文本");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "文本", PinType::String, "Hello, World!");
    node.Inputs.emplace_back(GetNextId(), "位置", PinType::Point, cv::Point(512, 512));
    node.Inputs.emplace_back(GetNextId(), "字体", PinType::Int, cv::FONT_HERSHEY_SIMPLEX);
    node.Inputs.emplace_back(GetNextId(), "字体大小", PinType::Float, 1.0f);
    node.Inputs.emplace_back(GetNextId(), "颜色", PinType::Color, cv::Scalar(0, 0, 0, 0));
    node.Inputs.emplace_back(GetNextId(), "线宽", PinType::Int, 1);
    node.Inputs.emplace_back(GetNextId(), "抗锯齿类型", PinType::Int, cv::LINE_8);
    node.Inputs.emplace_back(GetNextId(), "以左下角为原点", PinType::Bool, false);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        std::string text = "Hello, World!";
        get_value(graph, node->Inputs[1], text);

        cv::Point org(512, 512);
        get_value(graph, node->Inputs[2], org);

        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        get_value(graph, node->Inputs[3], fontFace);

        float fontScale = 1.0;
        get_value(graph, node->Inputs[4], fontScale);

        cv::Scalar color(0, 0, 0, 0);
        get_value(graph, node->Inputs[5], color);

        int thickness = 1;
        get_value(graph, node->Inputs[6], thickness);

        int lineType = cv::LINE_8;
        get_value(graph, node->Inputs[7], lineType);

        bool bottomLeftOrigin = false;
        get_value(graph, node->Inputs[8], bottomLeftOrigin);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = text;
        node->Inputs[2].Value = org;
        node->Inputs[3].Value = fontFace;
        node->Inputs[4].Value = fontScale;
        node->Inputs[5].Value = color;
        node->Inputs[6].Value = thickness;
        node->Inputs[7].Value = lineType;
        node->Inputs[8].Value = bottomLeftOrigin;

        try_catch_block
        {
            cv::Mat result = image.clone();
            cv::putText(result, text, org, fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageDrawNodes = {
    {"画线", Spawn_ImageOperator_DrawLine},
    {"画矩形", Spawn_ImageOperator_DrawRectangle},
    {"画圆", Spawn_ImageOperator_DrawCircle},
    {"画椭圆", Spawn_ImageOperator_DrawEllipse},
    {"画文本", Spawn_ImageOperator_DrawText},
};