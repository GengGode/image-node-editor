#pragma once

#include "base_nodes.hpp"

// 图像归一化
Node *Spawn_ImageNormalize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像归一化");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "图像");
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "归一化类型", cv::NORM_MINMAX);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "最小值", 0.0f);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "最大值", 255.0f);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "目标位宽", 8);

    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int type;
        get_value(graph, node->Inputs[1], type);

        float min_value;
        get_value(graph, node->Inputs[2], min_value);

        float max_value;
        get_value(graph, node->Inputs[3], max_value);

        int target_depth;
        get_value(graph, node->Inputs[4], target_depth);

        try_catch_block;

        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图像为空");

        int depth = CV_8U;
        switch (target_depth)
        {
        case 8:
            depth = CV_8U;
            break;
        case 16:
            depth = CV_16U;
            break;
        case 32:
            depth = CV_32F;
            break;
        case 64:
            depth = CV_64F;
            break;
        default:
            return ExecuteResult::ErrorNode(node->ID, "不支持的位宽");
        }

        cv::Mat result;
        cv::normalize(image, result, min_value, max_value, type, depth);

        node->Outputs[0].SetValue(result);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 图像转换类型
Node *Spawn_ImageConvertToType(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像转换类型");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "图像");
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "目标位宽", 8);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "乘数", 1.0f);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "加数", 0.0f);

    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int type;
        get_value(graph, node->Inputs[1], type);

        float multiplier;
        get_value(graph, node->Inputs[2], multiplier);

        float adder;
        get_value(graph, node->Inputs[3], adder);

        try_catch_block;

        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图像为空");

        int depth = CV_8U;
        switch (type)
        {
        case 8:
            depth = CV_8U;
            break;
        case 16:
            depth = CV_16U;
            break;
        case 32:
            depth = CV_32F;
            break;
        case 64:
            depth = CV_64F;
            break;
        default:
            return ExecuteResult::ErrorNode(node->ID, "不支持的位宽");
        }

        cv::Mat result;
        image.convertTo(result, depth, multiplier, adder);

        node->Outputs[0].SetValue(result);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 图像转换缩放 convertScaleAbs
Node *Spawn_ImageConvertScaleAbs(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像转换缩放");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "图像");
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "缩放比例", 1.0f);
    node.Inputs.emplace_back(GetNextId(), PinType::Float, "偏移", 0.0f);

    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        float scale;
        get_value(graph, node->Inputs[1], scale);

        float offset;
        get_value(graph, node->Inputs[2], offset);

        try_catch_block;

        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图像为空");

        cv::Mat result;
        cv::convertScaleAbs(image, result, scale, offset);

        node->Outputs[0].SetValue(result);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 图像转换到Fp16
Node *Spawn_ImageConvertToFp16(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像转换到Fp16");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "图像");

    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        try_catch_block;

        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图像为空");

        cv::Mat result;
        cv::convertFp16(image, result);

        node->Outputs[0].SetValue(result);

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// rgb to bgr
Node *Spawn_ImageOperator_RgbToBgr(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "RGB 转 BGR");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "BGR", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

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
    m_Nodes.emplace_back(GetNextId(), "RGBA 转 RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "RGBA", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        if (image.channels() != 4)
            return ExecuteResult::ErrorNode(node->ID, "Image must have 4 channels");

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
    m_Nodes.emplace_back(GetNextId(), "BGR 转 RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "BGR", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

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
    m_Nodes.emplace_back(GetNextId(), "灰度 转 RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "灰度", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

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
Node *Spawn_ImageOperator_ImageToGray(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像 转 灰度");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "灰度", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

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

    m_Nodes.emplace_back(GetNextId(), "RGB 转 HSV");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "HSV", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat rgb;
        get_value(graph, node->Inputs[0], rgb);

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

    m_Nodes.emplace_back(GetNextId(), "HSV 转 RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "HSV", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat hsv;
        get_value(graph, node->Inputs[0], hsv);

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

    m_Nodes.emplace_back(GetNextId(), "RGB 转 LAB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "LAB", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat rgb;
        get_value(graph, node->Inputs[0], rgb);

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

    m_Nodes.emplace_back(GetNextId(), "LAB 转 RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "LAB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat lab;
        get_value(graph, node->Inputs[0], lab);

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

    m_Nodes.emplace_back(GetNextId(), "RGB 转 YUV");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "YUV", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat rgb;
        get_value(graph, node->Inputs[0], rgb);

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

    m_Nodes.emplace_back(GetNextId(), "YUV 转 RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "YUV", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat yuv;
        get_value(graph, node->Inputs[0], yuv);

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

    m_Nodes.emplace_back(GetNextId(), "RGB 转 YCrCb");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "RGB", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "YCrCb", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat rgb;
        get_value(graph, node->Inputs[0], rgb);

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

    m_Nodes.emplace_back(GetNextId(), "YCrCb 转 RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOther;
    node.Inputs.emplace_back(GetNextId(), "YCrCb", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "RGB", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat ycrcb;
        get_value(graph, node->Inputs[0], ycrcb);

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
    {"图像归一化", Spawn_ImageNormalize},
    {"图像转换类型", Spawn_ImageConvertToType},
    {"图像转换缩放", Spawn_ImageConvertScaleAbs},
    {"图像转换到Fp16", Spawn_ImageConvertToFp16},
    {"RGB 转 BGR", Spawn_ImageOperator_RgbToBgr},
    {"RGBA 转 RGB", Spawn_ImageOperator_RgbaToRgb},
    {"BGR 转 RGB", Spawn_ImageOperator_BgrToRgb},
    {"灰度 转 RGB", Spawn_ImageOperator_GrayToRGB},
    {"图像 转 灰度", Spawn_ImageOperator_ImageToGray},
    {"RGB 转 HSV", Spawn_ImageOperator_RGBToHSV},
    {"HSV 转 RGB", Spawn_ImageOperator_HSVToRGB},
    {"RGB 转 LAB", Spawn_ImageOperator_RGBToLAB},
    {"LAB 转 RGB", Spawn_ImageOperator_LABToRGB},
    {"RGB 转 YUV", Spawn_ImageOperator_RGBToYUV},
    {"YUV 转 RGB", Spawn_ImageOperator_YUVToRGB},
    {"RGB 转 YCrCb", Spawn_ImageOperator_RGBToYCrCb},
    {"YCrCb 转 RGB", Spawn_ImageOperator_YCrCbToRGB},
};
