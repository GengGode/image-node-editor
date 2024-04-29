#pragma once

#include "base_nodes.hpp"

// rgb to bgr
Node *Spawn_ImageOperator_RgbToBgr(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "RGB to BGR");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat bgr;
            if (image.channels() == 3)
                cv::cvtColor(image, bgr, cv::COLOR_RGB2BGR);
            else if (image.channels() == 4)
                cv::cvtColor(image, bgr, cv::COLOR_RGBA2BGRA);
            node->Outputs[0].SetValue(bgr);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
Node *Spawn_ImageOperator_RgbaToRgb(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "RGBA to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        if (image.channels() != 4)
            return ExecuteResult::ErrorNode(node->ID, "Image must have 4 channels");

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat rgb;
            cv::cvtColor(image, rgb, cv::COLOR_RGBA2RGB);
            node->Outputs[0].SetValue(rgb);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
Node *Spawn_ImageOperator_BgrToRgb(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "BGR to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat rgb;
            if (image.channels() == 3)
                cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
            else if (image.channels() == 4)
                cv::cvtColor(image, rgb, cv::COLOR_BGRA2RGBA);
            node->Outputs[0].SetValue(rgb);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
Node *Spawn_ImageOperator_GrayToRGB(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Gray to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat rgb;
            if (image.channels() == 1)
                cv::cvtColor(image, rgb, cv::COLOR_GRAY2RGB);
            node->Outputs[0].SetValue(rgb);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
Node *Spawn_ImageOperator_Gray(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Gray");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Mat gray;
            if (image.channels() == 1)
                gray = image;
            else if (image.channels() == 3)
                cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
            else if (image.channels() == 4)
                cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
            node->Outputs[0].SetValue(gray);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// rgb to hsv
Node *Spawn_ImageOperator_RGBToHSV(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "RGB to HSV");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "HSV", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat rgb;
        auto result = get_image(graph, node->Inputs[0], rgb);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = rgb;

        try_catch_block;
        cv::Mat hsv;
        cv::cvtColor(rgb, hsv, cv::COLOR_RGB2HSV);
        node->Outputs[0].SetValue(hsv);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// hsv to rgb
Node *Spawn_ImageOperator_HSVToRGB(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "HSV to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "HSV", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat hsv;
        auto result = get_image(graph, node->Inputs[0], hsv);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = hsv;

        try_catch_block;
        cv::Mat rgb;
        cv::cvtColor(hsv, rgb, cv::COLOR_HSV2RGB);
        node->Outputs[0].SetValue(rgb);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// rgb to lab
Node *Spawn_ImageOperator_RGBToLAB(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "RGB to LAB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "LAB", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat rgb;
        auto result = get_image(graph, node->Inputs[0], rgb);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = rgb;

        try_catch_block;
        cv::Mat lab;
        cv::cvtColor(rgb, lab, cv::COLOR_RGB2Lab);
        node->Outputs[0].SetValue(lab);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// lab to rgb
Node *Spawn_ImageOperator_LABToRGB(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "LAB to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "LAB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat lab;
        auto result = get_image(graph, node->Inputs[0], lab);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = lab;

        try_catch_block;
        cv::Mat rgb;
        cv::cvtColor(lab, rgb, cv::COLOR_Lab2RGB);
        node->Outputs[0].SetValue(rgb);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// rgb to yuv
Node *Spawn_ImageOperator_RGBToYUV(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "RGB to YUV");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "YUV", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat rgb;
        auto result = get_image(graph, node->Inputs[0], rgb);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = rgb;

        try_catch_block;
        cv::Mat yuv;
        cv::cvtColor(rgb, yuv, cv::COLOR_RGB2YUV);
        node->Outputs[0].SetValue(yuv);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// yuv to rgb
Node *Spawn_ImageOperator_YUVToRGB(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "YUV to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "YUV", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat yuv;
        auto result = get_image(graph, node->Inputs[0], yuv);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = yuv;

        try_catch_block;
        cv::Mat rgb;
        cv::cvtColor(yuv, rgb, cv::COLOR_YUV2RGB);
        node->Outputs[0].SetValue(rgb);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// rgb to ycrcb
Node *Spawn_ImageOperator_RGBToYCrCb(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "RGB to YCrCb");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "YCrCb", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat rgb;
        auto result = get_image(graph, node->Inputs[0], rgb);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = rgb;

        try_catch_block;
        cv::Mat ycrcb;
        cv::cvtColor(rgb, ycrcb, cv::COLOR_RGB2YCrCb);
        node->Outputs[0].SetValue(ycrcb);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// ycrcb to rgb
Node *Spawn_ImageOperator_YCrCbToRGB(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "YCrCb to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "YCrCb", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat ycrcb;
        auto result = get_image(graph, node->Inputs[0], ycrcb);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = ycrcb;

        try_catch_block;
        cv::Mat rgb;
        cv::cvtColor(ycrcb, rgb, cv::COLOR_YCrCb2RGB);
        node->Outputs[0].SetValue(rgb);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageColorConvertNodes = {
    {"RGB to BGR", Spawn_ImageOperator_RgbToBgr},
    {"RGBA to RGB", Spawn_ImageOperator_RgbaToRgb},
    {"BGR to RGB", Spawn_ImageOperator_BgrToRgb},
    {"Gray to RGB", Spawn_ImageOperator_GrayToRGB},
    {"Gray", Spawn_ImageOperator_Gray},
    {"RGB to HSV", Spawn_ImageOperator_RGBToHSV},
    {"HSV to RGB", Spawn_ImageOperator_HSVToRGB},
    {"RGB to LAB", Spawn_ImageOperator_RGBToLAB},
    {"LAB to RGB", Spawn_ImageOperator_LABToRGB},
    {"RGB to YUV", Spawn_ImageOperator_RGBToYUV},
    {"YUV to RGB", Spawn_ImageOperator_YUVToRGB},
    {"RGB to YCrCb", Spawn_ImageOperator_RGBToYCrCb},
    {"YCrCb to RGB", Spawn_ImageOperator_YCrCbToRGB},
};
