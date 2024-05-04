#pragma once
#include "base_nodes.hpp"

// 一些轮廓相关处理
// EdgeDetection
Node *Spawn_ImageOperator_EdgeDetection(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Canny轮廓检测");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat gray, edge;
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, edge, 50, 150);

            node->Outputs[0].SetValue(edge);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// SobelEdgeDetection
Node *Spawn_ImageOperator_SobelEdgeDetection(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Sobel轮廓检测");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat gray, edge;
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

            cv::Mat grad_x, grad_y;
            cv::Mat abs_grad_x, abs_grad_y;
            cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
            cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
            cv::convertScaleAbs(grad_x, abs_grad_x);
            cv::convertScaleAbs(grad_y, abs_grad_y);
            cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edge);

            node->Outputs[0].SetValue(edge);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// LaplacianEdgeDetection
Node *Spawn_ImageOperator_LaplacianEdgeDetection(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Laplacian轮廓检测");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat gray, edge;
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
            cv::Laplacian(gray, edge, CV_16S, 3, 1, 0, cv::BORDER_DEFAULT);
            cv::convertScaleAbs(edge, edge);

            node->Outputs[0].SetValue(edge);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// ScharrEdgeDetection
Node *Spawn_ImageOperator_ScharrEdgeDetection(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Scharr轮廓检测");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat gray, edge;
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

            cv::Mat grad_x, grad_y;
            cv::Mat abs_grad_x, abs_grad_y;
            cv::Scharr(gray, grad_x, CV_16S, 1, 0, 1, 0, cv::BORDER_DEFAULT);
            cv::Scharr(gray, grad_y, CV_16S, 0, 1, 1, 0, cv::BORDER_DEFAULT);
            cv::convertScaleAbs(grad_x, abs_grad_x);
            cv::convertScaleAbs(grad_y, abs_grad_y);
            cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edge);

            node->Outputs[0].SetValue(edge);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 查找轮廓
Node *Spawn_ImageOperator_FindContours(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "查找轮廓");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "轮廓数组", PinType::Contours);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(image, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

            node->Outputs[0].SetValue(contours);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 绘制轮廓
Node *Spawn_ImageOperator_DrawContours(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "绘制轮廓");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "轮廓数组", PinType::Contours);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        std::vector<std::vector<cv::Point>> contours;
        auto result2 = get_value(graph, node->Inputs[1], contours);
        if (result2.has_error())
            return result2;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = contours;

        try_catch_block
        {
            cv::drawContours(image, contours, -1, cv::Scalar(0, 255, 0), 2);

            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 排序轮廓
Node *Spawn_ImageOperator_SortContoursByArea(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "排序轮廓-基于面具");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "轮廓数组", PinType::Contours);
    node.Outputs.emplace_back(GetNextId(), "轮廓数组", PinType::Contours);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        std::vector<std::vector<cv::Point>> contours;
        auto result = get_value(graph, node->Inputs[0], contours);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = contours;

        try_catch_block
        {
            std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b)
                      { return cv::contourArea(a) > cv::contourArea(b); });

            node->Outputs[0].SetValue(contours);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 过滤轮廓-基于面积
Node *Spawn_ImageOperator_FilterContoursByAreaRange(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "过滤轮廓-基于面积");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Contours", PinType::Contours);
    node.Inputs.emplace_back(GetNextId(), "Min Area", PinType::Int);
    node.Inputs.emplace_back(GetNextId(), "Max Area", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Contours", PinType::Contours);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        std::vector<std::vector<cv::Point>> contours;
        auto result = get_value(graph, node->Inputs[0], contours);
        if (result.has_error())
            return result;

        int min_area = 0;
        auto result2 = get_value(graph, node->Inputs[1], min_area);
        if (result2.has_error())
            return result2;

        int max_area = 0;
        auto result3 = get_value(graph, node->Inputs[2], max_area);
        if (result3.has_error())
            return result3;

        // Display image
        node->Inputs[0].Value = contours;
        node->Inputs[1].Value = min_area;
        node->Inputs[2].Value = max_area;

        try_catch_block
        {
            std::vector<std::vector<cv::Point>> filtered_contours;
            for (auto &contour : contours)
            {
                double area = cv::contourArea(contour);
                if (area >= min_area && area <= max_area)
                    filtered_contours.push_back(contour);
            }

            node->Outputs[0].SetValue(filtered_contours);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// 选择轮廓中的一个
Node *Spawn_ImageOperator_SelectContourByIndex(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "选择轮廓-基于索引");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Contours", PinType::Contours);
    node.Inputs.emplace_back(GetNextId(), "Index", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Contour", PinType::Contours);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        std::vector<std::vector<cv::Point>> contours;
        auto result = get_value(graph, node->Inputs[0], contours);
        if (result.has_error())
            return result;

        int index = 0;
        auto result2 = get_value(graph, node->Inputs[1], index);
        if (result2.has_error())
            return result2;

        // Display image
        node->Inputs[0].Value = contours;
        node->Inputs[1].Value = index;

        try_catch_block
        {
            if (index >= 0 && index < contours.size())
            {
                std::vector<std::vector<cv::Point>> selected_contour;
                selected_contour.push_back(contours[index]);
                node->Outputs[0].SetValue(selected_contour);
            }
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 霍夫变换查找圆
Node *Spawn_ImageOperator_HoughCircleDetection(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "霍夫圆查找");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "模式", PinType::Int, cv::HOUGH_GRADIENT);
    node.Inputs.emplace_back(GetNextId(), "dp", PinType::Float, 1.0f);
    node.Inputs.emplace_back(GetNextId(), "最小距离", PinType::Float, 20.0f);
    node.Inputs.emplace_back(GetNextId(), "param1", PinType::Float, 100.0f);
    node.Inputs.emplace_back(GetNextId(), "param2", PinType::Float, 100.0f);
    node.Inputs.emplace_back(GetNextId(), "minRadius", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "maxRadius", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Circles", PinType::Circles);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        int method = cv::HOUGH_GRADIENT;
        get_value(graph, node->Inputs[1], method);
        float dp = 0;
        get_value(graph, node->Inputs[2], dp);
        float minDist = 0;
        get_value(graph, node->Inputs[3], minDist);
        float param1 = 0;
        get_value(graph, node->Inputs[4], param1);
        float param2 = 0;
        get_value(graph, node->Inputs[5], param2);
        int minRadius = 0;
        get_value(graph, node->Inputs[6], minRadius);
        int maxRadius = 0;
        get_value(graph, node->Inputs[7], maxRadius);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = method;
        node->Inputs[2].Value = dp;
        node->Inputs[3].Value = minDist;
        node->Inputs[4].Value = param1;
        node->Inputs[5].Value = param2;
        node->Inputs[6].Value = minRadius;
        node->Inputs[7].Value = maxRadius;

        try_catch_block
        {
            std::vector<cv::Vec3f> circles;
            cv::HoughCircles(image, circles, method, dp, minDist, param1, param2, minRadius, maxRadius);

            node->Outputs[0].SetValue(circles);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageOperatorEdgeNodes = {
    {"Canny轮廓检测", Spawn_ImageOperator_EdgeDetection},
    {"Sobel轮廓检测", Spawn_ImageOperator_SobelEdgeDetection},
    {"Laplacian轮廓检测", Spawn_ImageOperator_LaplacianEdgeDetection},
    {"Scharr轮廓检测", Spawn_ImageOperator_ScharrEdgeDetection},
    {"查找轮廓", Spawn_ImageOperator_FindContours},
    {"绘制轮廓", Spawn_ImageOperator_DrawContours},
    {"排序轮廓-基于面积", Spawn_ImageOperator_SortContoursByArea},
    {"过滤轮廓-基于面积", Spawn_ImageOperator_FilterContoursByAreaRange},
    {"选择轮廓-基于索引", Spawn_ImageOperator_SelectContourByIndex},
    {"霍夫圆查找", Spawn_ImageOperator_HoughCircleDetection},

};