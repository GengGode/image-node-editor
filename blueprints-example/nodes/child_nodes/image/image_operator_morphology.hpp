#pragma once
#include "base_nodes.hpp"

// 膨胀运算
Node *Spawn_ImageOperator_Dilate(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Dilate");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Kernel Size", PinType::Int, 3);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        result = get_value(graph, node->Inputs[1], kernel_size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;

        try_catch_block
        {
            cv::Mat dilate;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::dilate(image, dilate, element);
            node->Outputs[0].SetValue(dilate);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 腐蚀运算
Node *Spawn_ImageOperator_Erode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Erode");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Kernel Size", PinType::Int, 3);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        result = get_value(graph, node->Inputs[1], kernel_size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;

        try_catch_block
        {
            cv::Mat erode;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::erode(image, erode, element);
            node->Outputs[0].SetValue(erode);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 开运算
Node *Spawn_ImageOperator_MorphologyOpen(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Morphology Open");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Kernel Size", PinType::Int, 3);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        result = get_value(graph, node->Inputs[1], kernel_size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;

        try_catch_block
        {
            cv::Mat open;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, open, cv::MORPH_OPEN, element);
            node->Outputs[0].SetValue(open);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 闭运算
Node *Spawn_ImageOperator_MorphologyClose(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Morphology Close");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Kernel Size", PinType::Int, 3);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        result = get_value(graph, node->Inputs[1], kernel_size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;

        try_catch_block
        {
            cv::Mat close;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, close, cv::MORPH_CLOSE, element);
            node->Outputs[0].SetValue(close);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 形态学梯度
Node *Spawn_ImageOperator_MorphologyGradient(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Morphology Gradient");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Kernel Size", PinType::Int, 3);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        result = get_value(graph, node->Inputs[1], kernel_size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;

        try_catch_block
        {
            cv::Mat gradient;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, gradient, cv::MORPH_GRADIENT, element);
            node->Outputs[0].SetValue(gradient);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 顶帽运算
Node *Spawn_ImageOperator_MorphologyTopHat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Morphology Top Hat");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Kernel Size", PinType::Int, 3);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        result = get_value(graph, node->Inputs[1], kernel_size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;

        try_catch_block
        {
            cv::Mat top_hat;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, top_hat, cv::MORPH_TOPHAT, element);
            node->Outputs[0].SetValue(top_hat);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 黑帽运算
Node *Spawn_ImageOperator_MorphologyBlackHat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Morphology Black Hat");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Kernel Size", PinType::Int, 3);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int kernel_size = 3;
        result = get_value(graph, node->Inputs[1], kernel_size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = kernel_size;

        try_catch_block
        {
            cv::Mat black_hat;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, black_hat, cv::MORPH_BLACKHAT, element);
            node->Outputs[0].SetValue(black_hat);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageOperatorMorphologyNodes = {
    {"Dilate", Spawn_ImageOperator_Dilate},
    {"Erode", Spawn_ImageOperator_Erode},
    {"Morphology Open", Spawn_ImageOperator_MorphologyOpen},
    {"Morphology Close", Spawn_ImageOperator_MorphologyClose},
    {"Morphology Gradient", Spawn_ImageOperator_MorphologyGradient},
    {"Morphology Top Hat", Spawn_ImageOperator_MorphologyTopHat},
    {"Morphology Black Hat", Spawn_ImageOperator_MorphologyBlackHat},
};