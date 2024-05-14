#pragma once
#include "base_nodes.hpp"

// 一些轮廓相关处理
// canny
Node *Spawn_ImageOperator_Canny(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Canny轮廓检测");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOperation_Edge;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Threshold 1", PinType::Float, 50.0f);
    node.Inputs.emplace_back(GetNextId(), "Threshold 2", PinType::Float, 150.0f);
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Int, 3);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        float threshold_1 = 50;
        get_value(graph, node->Inputs[1], threshold_1);
        float threshold_2 = 150;
        get_value(graph, node->Inputs[2], threshold_2);
        int size = 3;
        get_value(graph, node->Inputs[3], size);

        try_catch_block
        {
            if (size % 2 == 0)
                return ExecuteResult::ErrorNode(node->ID, "Size must be odd");
            cv::Mat result;
            cv::Canny(image, result, threshold_1, threshold_2, size);
            node->Outputs[0].SetValue(result);
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
    node.Type = NodeType::ImageOperation_Edge;
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
            cv::Mat result;
            cv::Mat grad_x, grad_y;
            cv::Mat abs_grad_x, abs_grad_y;
            cv::Sobel(image, grad_x, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
            cv::Sobel(image, grad_y, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
            cv::convertScaleAbs(grad_x, abs_grad_x);
            cv::convertScaleAbs(grad_y, abs_grad_y);
            cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, result);

            node->Outputs[0].SetValue(result);
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
    node.Type = NodeType::ImageOperation_Edge;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "深度", CV_16S);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "K大小", 3);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "比例", 1.0f);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "增量", 0.0f);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "边缘填充", cv::BORDER_DEFAULT);

    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        
        int ddepth = CV_16S;
        get_value(graph, node->Inputs[1], ddepth);
        int ksize = 3;
        get_value(graph, node->Inputs[2], ksize);
        float scale = 1.0f;
        get_value(graph, node->Inputs[3], scale);
        float delta = 0.0f;
        get_value(graph, node->Inputs[4], delta);
        int borderType = cv::BORDER_DEFAULT;
        get_value(graph, node->Inputs[5], borderType);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = ddepth;
        node->Inputs[2].Value = ksize;
        node->Inputs[3].Value = scale;
        node->Inputs[4].Value = delta;
        node->Inputs[5].Value = borderType;
        
        try_catch_block
        {
            cv::Mat result;
            cv::Laplacian(image, result, ddepth, ksize, scale, delta, borderType);
            //cv::convertScaleAbs(result, result);

            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// Laplacian边缘增强
Node *Spawn_ImageOperator_LaplacianEdgeEnhancement(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Laplacian边缘增强");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOperation_Edge;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "深度", CV_16S);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "K大小", 3);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "比例", 1.0f);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "增量", 0.0f);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "边缘填充", cv::BORDER_DEFAULT);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "增强系数", 1.0f);
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int ddepth = CV_16S;
        get_value(graph, node->Inputs[1], ddepth);
        int ksize = 3;
        get_value(graph, node->Inputs[2], ksize);
        float scale = 1.0f;
        get_value(graph, node->Inputs[3], scale);
        float delta = 0.0f;
        get_value(graph, node->Inputs[4], delta);
        int borderType = cv::BORDER_DEFAULT;
        get_value(graph, node->Inputs[5], borderType);
        float alpha = 1.0f;
        get_value(graph, node->Inputs[6], alpha);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = ddepth;
        node->Inputs[2].Value = ksize;
        node->Inputs[3].Value = scale;
        node->Inputs[4].Value = delta;
        node->Inputs[5].Value = borderType;
        node->Inputs[6].Value = alpha;

        try_catch_block
        {
            cv::Mat result;
            cv::Mat laplacian;
            cv::Laplacian(image, laplacian, ddepth, ksize, scale, delta, borderType);
            cv::convertScaleAbs(laplacian, laplacian);
            cv::addWeighted(image, 1.0, laplacian, alpha, 0, result);

            node->Outputs[0].SetValue(result);
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
    node.Type = NodeType::ImageOperation_Edge;
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
            cv::Mat result;
            cv::Mat grad_x, grad_y;
            cv::Mat abs_grad_x, abs_grad_y;
            cv::Scharr(image, grad_x, CV_16S, 1, 0, 1, 0, cv::BORDER_DEFAULT);
            cv::Scharr(image, grad_y, CV_16S, 0, 1, 1, 0, cv::BORDER_DEFAULT);
            cv::convertScaleAbs(grad_x, abs_grad_x);
            cv::convertScaleAbs(grad_y, abs_grad_y);
            cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, result);

            node->Outputs[0].SetValue(result);
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
    node.Type = NodeType::ImageOperation_Edge;
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
    node.Type = NodeType::ImageOperation_Edge;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "轮廓数组", PinType::Contours);
    node.Inputs.emplace_back(GetNextId(), "绘制颜色", PinType::Color);
    node.Inputs.emplace_back(GetNextId(), "随机颜色", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "线宽", PinType::Int, 2);
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
        cv::Scalar color;
        get_value(graph, node->Inputs[2], color);

        bool random_color = false;
        get_value(graph, node->Inputs[3], random_color);

        int thickness = 2;
        get_value(graph, node->Inputs[4], thickness);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = contours;
        node->Inputs[2].Value = color;
        node->Inputs[3].Value = random_color;
        node->Inputs[4].Value = thickness;

        try_catch_block
        {
            cv::Mat result = image.clone();
            if (random_color)
            {
                for (size_t i = 0; i < contours.size(); i++)
                {
                    cv::Scalar color(rand() & 255, rand() & 255, rand() & 255);
                    cv::drawContours(result, contours, static_cast<int>(i), color, thickness);
                }
            }
            else
            {
                cv::drawContours(result, contours, -1, color, thickness);
            }
            node->Outputs[0].SetValue(result);
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
    node.Type = NodeType::ImageOperation_Edge;
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
    node.Type = NodeType::ImageOperation_Edge;
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
    node.Type = NodeType::ImageOperation_Edge;
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
    node.Type = NodeType::ImageOperation_Edge;
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

// 绘制霍夫圆
Node *Spawn_ImageOperator_DrawHoughCircles(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "绘制霍夫圆");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOperation_Edge;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Circles", PinType::Circles);
    node.Inputs.emplace_back(GetNextId(), "绘制颜色", PinType::Color);
    node.Inputs.emplace_back(GetNextId(), "线宽", PinType::Int, 2);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        std::vector<cv::Vec3f> circles;
        auto result2 = get_value(graph, node->Inputs[1], circles);
        if (result2.has_error())
            return result2;

        cv::Scalar color;
        get_value(graph, node->Inputs[2], color);

        int thickness = 2;
        get_value(graph, node->Inputs[3], thickness);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = circles;
        node->Inputs[2].Value = color;
        node->Inputs[3].Value = thickness;

        try_catch_block
        {
            for (size_t i = 0; i < circles.size(); i++)
            {
                cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
                int radius = cvRound(circles[i][2]);
                cv::circle(image, center, radius, color, thickness);
            }

            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageOperatorEdgeNodes = {
    {"Canny轮廓检测", Spawn_ImageOperator_Canny},
    {"Sobel轮廓检测", Spawn_ImageOperator_SobelEdgeDetection},
    {"Laplacian轮廓检测", Spawn_ImageOperator_LaplacianEdgeDetection},
    {"Laplacian边缘增强", Spawn_ImageOperator_LaplacianEdgeEnhancement},
    {"Scharr轮廓检测", Spawn_ImageOperator_ScharrEdgeDetection},
    {"查找轮廓", Spawn_ImageOperator_FindContours},
    {"绘制轮廓", Spawn_ImageOperator_DrawContours},
    {"排序轮廓-基于面积", Spawn_ImageOperator_SortContoursByArea},
    {"过滤轮廓-基于面积", Spawn_ImageOperator_FilterContoursByAreaRange},
    {"选择轮廓-基于索引", Spawn_ImageOperator_SelectContourByIndex},
    {"霍夫圆查找", Spawn_ImageOperator_HoughCircleDetection},
    {"绘制霍夫圆", Spawn_ImageOperator_DrawHoughCircles},
};