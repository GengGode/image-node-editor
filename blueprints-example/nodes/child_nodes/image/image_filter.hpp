#pragma once
#include "base_nodes.hpp"

#include <opencv2/xphoto.hpp>

// 滤波算法

// 低通滤波
Node *Spawn_ImageOperator_LowPassFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "低通滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "卷积核大小", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "卷积核类型", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "边界填充类型", PinType::Int, cv::BORDER_DEFAULT);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        get_value(graph, node->Inputs[1], kernel_size);

        int kernel_type = 0;
        get_value(graph, node->Inputs[2], kernel_type);

        int border_type = cv::BORDER_DEFAULT;
        get_value(graph, node->Inputs[3], border_type);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;
        node->Inputs[2].Value = kernel_type;
        node->Inputs[3].Value = border_type;

        try_catch_block
        {
            cv::Mat kernel = cv::Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size);
            cv::filter2D(image, image, -1, kernel, cv::Point(-1, -1), 0, border_type);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 高通滤波
Node *Spawn_ImageOperator_HighPassFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "高通滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "卷积核大小", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "卷积核类型", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "边界填充类型", PinType::Int, cv::BORDER_DEFAULT);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        get_value(graph, node->Inputs[1], kernel_size);

        int kernel_type = 0;
        get_value(graph, node->Inputs[2], kernel_type);

        int border_type = cv::BORDER_DEFAULT;
        get_value(graph, node->Inputs[3], border_type);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;
        node->Inputs[2].Value = kernel_type;
        node->Inputs[3].Value = border_type;

        try_catch_block
        {
            cv::Mat kernel = cv::Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size * kernel_size);
            cv::Mat dst;
            cv::filter2D(image, dst, -1, kernel, cv::Point(-1, -1), 0, border_type);
            cv::subtract(image, dst, image);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 方框滤波
Node *Spawn_ImageOperator_BoxFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "方框滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "卷积核大小", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "边界填充类型", PinType::Int, cv::BORDER_DEFAULT);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        get_value(graph, node->Inputs[1], kernel_size);

        int border_type = cv::BORDER_DEFAULT;
        get_value(graph, node->Inputs[2], border_type);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;
        node->Inputs[2].Value = border_type;

        try_catch_block
        {
            cv::boxFilter(image, image, -1, cv::Size(kernel_size, kernel_size), cv::Point(-1, -1), true, border_type);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 均值滤波
Node *Spawn_ImageOperator_BlurFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "均值滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "卷积核大小", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "边界填充类型", PinType::Int, cv::BORDER_DEFAULT);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        get_value(graph, node->Inputs[1], kernel_size);

        int border_type = cv::BORDER_DEFAULT;
        get_value(graph, node->Inputs[2], border_type);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;
        node->Inputs[2].Value = border_type;

        try_catch_block
        {
            cv::blur(image, image, cv::Size(kernel_size, kernel_size), cv::Point(-1, -1), border_type);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 高斯滤波
Node *Spawn_ImageOperator_GaussianFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "高斯滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "卷积核大小", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "标准差", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "边界填充类型", PinType::Int, (int)cv::BORDER_DEFAULT);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        get_value(graph, node->Inputs[1], kernel_size);

        float sigma = 0.0;
        get_value(graph, node->Inputs[2], sigma);

        int border_type = cv::BORDER_DEFAULT;
        get_value(graph, node->Inputs[3], border_type);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;
        node->Inputs[2].Value = sigma;
        node->Inputs[3].Value = border_type;

        try_catch_block
        {
            cv::GaussianBlur(image, image, cv::Size(kernel_size, kernel_size), sigma, sigma, border_type);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 中值滤波
Node *Spawn_ImageOperator_MedianFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "中值滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "卷积核大小", PinType::Int, 3);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        get_value(graph, node->Inputs[1], kernel_size);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;

        try_catch_block
        {
            cv::medianBlur(image, image, kernel_size);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 双边滤波
Node *Spawn_ImageOperator_BilateralFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "双边滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "卷积核大小", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "颜色空间标准差", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "坐标空间标准差", PinType::Float, 0.0f);
    node.Inputs.emplace_back(GetNextId(), "边界填充类型", PinType::Int, (int)cv::BORDER_DEFAULT);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        get_value(graph, node->Inputs[1], kernel_size);

        float sigma_color = 0.0;
        get_value(graph, node->Inputs[2], sigma_color);

        float sigma_space = 0.0;
        get_value(graph, node->Inputs[3], sigma_space);

        int border_type = cv::BORDER_DEFAULT;
        get_value(graph, node->Inputs[4], border_type);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;
        node->Inputs[2].Value = sigma_color;
        node->Inputs[3].Value = sigma_space;
        node->Inputs[4].Value = border_type;

        try_catch_block
        {
            cv::bilateralFilter(image, image, kernel_size, sigma_color, sigma_space, border_type);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 非局部均值滤波
Node *Spawn_ImageOperator_NonLocalMeansFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "非局部均值滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    /*float h = 3, float hColor = 3,
        int templateWindowSize = 7, int searchWindowSize = 21*/
    node.Inputs.emplace_back(GetNextId(), "h", PinType::Float, 3.0f);
    node.Inputs.emplace_back(GetNextId(), "hColor", PinType::Float, 3.0f);
    node.Inputs.emplace_back(GetNextId(), "模板窗口大小", PinType::Int, 7);
    node.Inputs.emplace_back(GetNextId(), "搜索窗口大小", PinType::Int, 21);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        float h = 3.0f;
        get_value(graph, node->Inputs[1], h);
        float hColor = 3.0f;
        get_value(graph, node->Inputs[2], hColor);
        int templateWindowSize = 7;
        get_value(graph, node->Inputs[3], templateWindowSize);
        int searchWindowSize = 21;
        get_value(graph, node->Inputs[4], searchWindowSize);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = h;
        node->Inputs[2].Value = hColor;
        node->Inputs[3].Value = templateWindowSize;
        node->Inputs[4].Value = searchWindowSize;

        try_catch_block
        {
            cv::fastNlMeansDenoisingColored(image, image, h, hColor, templateWindowSize, searchWindowSize);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 自适应均值滤波
Node *Spawn_ImageOperator_AdaptiveMeanFilter(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "自适应均值滤波");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    /*
            float h = 1,
            int templateWindowSize = 4,
            int searchWindowSize = 16,
            int blockMatchingStep1 = 2500,
            int blockMatchingStep2 = 400,
            int groupSize = 8,
            int slidingStep = 1,
            float beta = 2.0f,
            int normType = cv::NORM_L2,
            int step = cv::xphoto::BM3D_STEPALL,
            int transformType = cv::xphoto::HAAR
            */
    node.Inputs.emplace_back(GetNextId(), "颜色空间标准差", PinType::Float, 1.0f);
    node.Inputs.emplace_back(GetNextId(), "模板窗口大小", PinType::Int, 4);
    node.Inputs.emplace_back(GetNextId(), "搜索窗口大小", PinType::Int, 16);
    node.Inputs.emplace_back(GetNextId(), "块匹配步骤1", PinType::Int, 2500);
    node.Inputs.emplace_back(GetNextId(), "块匹配步骤2", PinType::Int, 400);
    node.Inputs.emplace_back(GetNextId(), "组大小", PinType::Int, 8);
    node.Inputs.emplace_back(GetNextId(), "滑动步长", PinType::Int, 1);
    node.Inputs.emplace_back(GetNextId(), "beta", PinType::Float, 2.0f);
    node.Inputs.emplace_back(GetNextId(), "范数类型", PinType::Int, (int)cv::NORM_L2);
    node.Inputs.emplace_back(GetNextId(), "步骤", PinType::Int, (int)cv::xphoto::BM3D_STEPALL);
    node.Inputs.emplace_back(GetNextId(), "变换类型", PinType::Int, (int)cv::xphoto::HAAR);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        float h = 1;
        get_value(graph, node->Inputs[1], h);
        int templateWindowSize = 4;
        get_value(graph, node->Inputs[2], templateWindowSize);
        int searchWindowSize = 16;
        get_value(graph, node->Inputs[3], searchWindowSize);
        int blockMatchingStep1 = 2500;
        get_value(graph, node->Inputs[4], blockMatchingStep1);
        int blockMatchingStep2 = 400;
        get_value(graph, node->Inputs[5], blockMatchingStep2);
        int groupSize = 8;
        get_value(graph, node->Inputs[6], groupSize);
        int slidingStep = 1;
        get_value(graph, node->Inputs[7], slidingStep);
        float beta = 2.0f;
        get_value(graph, node->Inputs[8], beta);
        int normType = cv::NORM_L2;
        get_value(graph, node->Inputs[9], normType);
        int step = cv::xphoto::BM3D_STEPALL;
        get_value(graph, node->Inputs[10], step);
        int transformType = cv::xphoto::HAAR;
        get_value(graph, node->Inputs[11], transformType);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = h;
        node->Inputs[2].Value = templateWindowSize;
        node->Inputs[3].Value = searchWindowSize;
        node->Inputs[4].Value = blockMatchingStep1;
        node->Inputs[5].Value = blockMatchingStep2;
        node->Inputs[6].Value = groupSize;
        node->Inputs[7].Value = slidingStep;
        node->Inputs[8].Value = beta;
        node->Inputs[9].Value = normType;
        node->Inputs[10].Value = step;
        node->Inputs[11].Value = transformType;

        try_catch_block
        {
            cv::xphoto::bm3dDenoising(image, image, h, templateWindowSize, searchWindowSize, blockMatchingStep1, blockMatchingStep2, groupSize, slidingStep, beta, normType, step, transformType);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageOperatorFilterNodes = {
    {"低通滤波", Spawn_ImageOperator_LowPassFilter},
    {"高通滤波", Spawn_ImageOperator_HighPassFilter},
    {"方框滤波", Spawn_ImageOperator_BoxFilter},
    {"均值滤波", Spawn_ImageOperator_BlurFilter},
    {"高斯滤波", Spawn_ImageOperator_GaussianFilter},
    {"中值滤波", Spawn_ImageOperator_MedianFilter},
    {"双边滤波", Spawn_ImageOperator_BilateralFilter},
    {"非局部均值滤波", Spawn_ImageOperator_NonLocalMeansFilter},
    {"自适应均值滤波", Spawn_ImageOperator_AdaptiveMeanFilter},
};