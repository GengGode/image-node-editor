#ifndef CHILD_NODES_HPP
#define CHILD_NODES_HPP

#include "../base_nodes.hpp"
#include "blueprint/blueprint.hpp"

#include "base/base_type.hpp"
#include "base/base_convert.hpp"
#include "base/base_operation.hpp"
#include "image/image_source.hpp"
#include "image/image_type.hpp"
#include "image/image_math.hpp"
#include "image/image_color_convert.hpp"
#include "image/image_filter.hpp"
// #include "image/image_histogram.hpp"
// #include "image/image_transform.hpp"
#include "image/image_draw.hpp"
// #include "image/image_io.hpp"
// #include "image/image_video.hpp"
// #include "image/image_camera.hpp"
#include "image/image_feature.hpp"
// #include "image/image_segmentation.hpp"
// #include "image/image_object_detection.hpp"
// #include "image/image_super_resolution.hpp"
// #include "image/image_style_transfer.hpp"
// #include "image/image_face_alignment.hpp"
// #include "image/image_face_detection.hpp"
// #include "image/image_face_recognition.hpp"
// #include "image/image_image_retrieval.hpp"
#include "image/image_operator_morphology.hpp"
#include "image/image_operator_edge.hpp"
#include "image/image_operator_threshold.hpp"

#include <filesystem>

#include <libocr.h>

Node *Spawn_ImageViewer(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像查看器");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图像为空");

        node->Inputs[0].SetValue(image);
        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

// 写入本地文件
Node *Spawn_ImageWriteLocalFile(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "写入本地文件");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "图像");
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文件路径", std::string("output.png"));
    node.Inputs.emplace_back(GetNextId(), PinType::Bool, "启用压缩", false);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "压缩参数", 3);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        std::string path;
        get_value(graph, node->Inputs[1], path);

        bool compress;
        get_value(graph, node->Inputs[2], compress);

        int compress_param;
        get_value(graph, node->Inputs[3], compress_param);

        node->Inputs[1].Value = path;
        node->Inputs[2].Value = compress;
        node->Inputs[3].Value = compress_param;

        try_catch_block;

        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图像为空");

        auto extension = std::filesystem::path(path).extension().string();
        if (extension == ".jpg" || extension == ".jpeg")
        {
            std::vector<int> params;
            params.push_back(cv::IMWRITE_JPEG_QUALITY);
            params.push_back(compress);
            cv::imwrite(path, image, params);
        }
        else if (extension == ".png")
        {
            std::vector<int> params;
            params.push_back(cv::IMWRITE_PNG_COMPRESSION);
            params.push_back(compress);
            cv::imwrite(path, image, params);
        }
        else
        {
            cv::imwrite(path, image);
        }

        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 写入Raw文件
Node *Spawn_ImageWriteRawFile(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "写入Raw文件");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "图像");
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文件路径", std::string("output.raw"));
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "通道字节数", 8);
    node.Inputs.emplace_back(GetNextId(), PinType::Bool, "小端序", false);
    node.Inputs.emplace_back(GetNextId(), PinType::String, "文件头", std::string());

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        std::string path;
        get_value(graph, node->Inputs[1], path);

        int depth;
        get_value(graph, node->Inputs[2], depth);

        bool little_endian;
        get_value(graph, node->Inputs[3], little_endian);

        std::string header;
        get_value(graph, node->Inputs[4], header);

        node->Inputs[1].Value = path;
        node->Inputs[2].Value = depth;
        node->Inputs[3].Value = little_endian;
        node->Inputs[4].Value = header;

        try_catch_block;

        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图像为空");

        int cv_depth = CV_8U;
        switch (depth)
        {
        case 8:
            cv_depth = CV_8U;
            break;
        case 16:
            cv_depth = CV_16U;
            break;
        case 32:
            cv_depth = CV_32F;
            break;
        case 64:
            cv_depth = CV_64F;
            break;
        default:
            return ExecuteResult::ErrorNode(node->ID, "不支持的深度");
        }

        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
            return ExecuteResult::ErrorNode(node->ID, "文件打开失败");

        if (header.size() > 0)
            file.write(header.c_str(), header.size());

        int image_size = image.cols * image.rows * image.channels() * static_cast<int>(image.elemSize1());
        switch (cv_depth)
        {
        case CV_8U:
        {
            std::vector<uint8_t> buffer(image_size);
            std::memcpy(buffer.data(), image.data, image_size);
            if (little_endian)
            {
                for (auto &v : buffer)
                    v = static_cast<uint8_t>(_byteswap_ulong(v));
            }
            file.write(reinterpret_cast<char *>(buffer.data()), image_size);
        }
        break;
        case CV_16U:
        {
            std::vector<uint16_t> buffer(image_size / 2);
            std::memcpy(buffer.data(), image.data, image_size);
            if (little_endian)
            {
                for (auto &v : buffer)
                    v = _byteswap_ushort(v);
            }
            file.write(reinterpret_cast<char *>(buffer.data()), image_size);
        }
        break;
        case CV_32F:
        {
            std::vector<int32_t> buffer(image_size / 4);
            std::memcpy(buffer.data(), image.data, image_size);
            if (little_endian)
            {
                for (auto &v : buffer)
                    v = _byteswap_ulong(v);
            }
            file.write(reinterpret_cast<char *>(buffer.data()), image_size);
        }
        break;
        case CV_64F:
        {
            std::vector<uint64_t> buffer(image_size / 8);
            std::memcpy(buffer.data(), image.data, image_size);
            if (little_endian)
            {
                for (auto &v : buffer)
                    v = _byteswap_uint64(v);
            }
            file.write(reinterpret_cast<char *>(buffer.data()), image_size);
        }
        break;
        default:
            return ExecuteResult::ErrorNode(node->ID, "不支持的字节宽度 " + std::to_string(depth));
        }

        catch_block_and_return;
    };
    BuildNode(&node);
    return &node;
}

// Image Get Size
Node *Spawn_ImageOperator_ImageGetSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取图像大小");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs.emplace_back(GetNextId(), PinType::Size);
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "Width");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "Height");

    node.Outputs[0].app = app;
    node.Outputs[1].app = app;
    node.Outputs[2].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            node->Outputs[0].SetValue(image.size());
            node->Outputs[1].SetValue(image.cols);
            node->Outputs[2].SetValue(image.rows);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// Image Get Rect
Node *Spawn_ImageOperator_ImageGetRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取图像范围");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), "X", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Y", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Width", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Height", PinType::Int);

    node.Outputs[0].app = app;
    node.Outputs[1].app = app;
    node.Outputs[2].app = app;
    node.Outputs[3].app = app;
    node.Outputs[4].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            cv::Rect rect(0, 0, image.cols, image.rows);
            node->Outputs[0].SetValue(rect);
            node->Outputs[1].SetValue(rect.x);
            node->Outputs[2].SetValue(rect.y);
            node->Outputs[3].SetValue(rect.width);
            node->Outputs[4].SetValue(rect.height);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// Image Get Channels
Node *Spawn_ImageOperator_ImageGetChannels(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取图像通道数");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Channels", PinType::Int);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            node->Outputs[0].SetValue(image.channels());
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// Image Get Depth
Node *Spawn_ImageOperator_ImageGetDepth(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取图像深度");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Depth", PinType::Int);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            node->Outputs[0].SetValue(image.depth());
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// Image Get All Info
Node *Spawn_ImageOperator_ImageGetAllInfo(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取图像信息");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "大小", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "中心点", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "通道数", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "位深", PinType::Int);

    node.Outputs[0].app = app;
    node.Outputs[1].app = app;
    node.Outputs[2].app = app;
    node.Outputs[3].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try_catch_block
        {
            node->Outputs[0].SetValue(image.size());
            node->Outputs[1].SetValue(cv::Point(image.cols / 2, image.rows / 2));
            node->Outputs[2].SetValue(image.channels());
            node->Outputs[3].SetValue(image.depth());
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// ImageGetRectImage
Node *Spawn_ImageOperator_ImageGetRectImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "调整图像大小");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Rect rect;
        get_value(graph, node->Inputs[1], rect);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = rect;

        try_catch_block
        {
            cv::Mat rect_image = image(rect).clone();
            node->Outputs[0].SetValue(rect_image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// Rect Image To Image
Node *Spawn_ImageOperator_RectImageToImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "范围图像覆盖图像");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Rect, "范围");
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "覆盖图像");

    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Rect rect;
        get_value(graph, node->Inputs[1], rect);

        cv::Mat overlay;
        result = get_image(graph, node->Inputs[2], overlay);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = rect;
        node->Inputs[2].Value = overlay;

        try_catch_block
        {
            if (image.empty())
                return ExecuteResult::ErrorNode(node->ID, "图像为空");

            if (overlay.empty())
                return ExecuteResult::ErrorNode(node->ID, "覆盖图像为空");

            if (rect.x < 0 || rect.y < 0 || rect.x + rect.width > image.cols || rect.y + rect.height > image.rows)
                return ExecuteResult::ErrorNode(node->ID, "范围超出图像范围");

            cv::Mat result_image = image.clone();
            overlay.copyTo(result_image(rect));
            node->Outputs[0].SetValue(result_image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

/* *** */
/* *** */
Node *Spawn_ImageOperator_ImageReSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "调整图像大小");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Width", PinType::Int, 640);
    node.Inputs.emplace_back(GetNextId(), "Height", PinType::Int, 480);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int width = 640;
        result = get_value(graph, node->Inputs[1], width);
        if (result.has_error())
            return result;

        int height = 480;
        result = get_value(graph, node->Inputs[2], height);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = width;
        node->Inputs[2].Value = height;

        try_catch_block
        {
            cv::Mat resize;
            cv::resize(image, resize, cv::Size(width, height));
            node->Outputs[0].SetValue(resize);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 遮罩运算
Node *Spawn_ImageOperator_MaskImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Mask Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Mask", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Mat mask;
        result = get_image(graph, node->Inputs[1], mask);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = mask;

        try_catch_block
        {
            cv::Mat masked_image;
            cv::bitwise_and(image, mask, masked_image);
            node->Outputs[0].SetValue(masked_image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// ImageChannelSplit
Node *Spawn_ImageOperator_ImageChannelSplit(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像通道拆分");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Channel 0", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Channel 1", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Channel 2", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Channel 3", PinType::Image);
    node.Outputs[0].app = app;
    node.Outputs[1].app = app;
    node.Outputs[2].app = app;
    node.Outputs[3].app = app;

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
            std::vector<cv::Mat> channels;
            cv::split(image, channels);
            if (channels.size() > 0)
                node->Outputs[0].SetValue(channels[0]);
            if (channels.size() > 1)
                node->Outputs[1].SetValue(channels[1]);
            if (channels.size() > 2)
                node->Outputs[2].SetValue(channels[2]);
            if (channels.size() > 3)
                node->Outputs[3].SetValue(channels[3]);
            if (channels.size() > 4)
                return ExecuteResult::ErrorNode(node->ID, "Too many channels");
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// ImageChannelMerge
Node *Spawn_ImageOperator_ImageChannelMerge(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像通道合并");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Channel 0", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Channel 1", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Channel 2", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Channel 3", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat channel0;
        auto result = get_image(graph, node->Inputs[0], channel0);
        if (result.has_error())
            return result;

        cv::Mat channel1;
        result = get_image(graph, node->Inputs[1], channel1);
        if (result.has_error())
            return result;

        cv::Mat channel2;
        result = get_image(graph, node->Inputs[2], channel2);
        if (result.has_error())
            return result;

        cv::Mat channel3;
        result = get_image(graph, node->Inputs[3], channel3);
        // note: channel3 is optional
        // if (result.has_error())
        //    return result;

        // Display image
        node->Inputs[0].Value = channel0;
        node->Inputs[1].Value = channel1;
        node->Inputs[2].Value = channel2;
        node->Inputs[3].Value = channel3;

        try_catch_block
        {
            cv::Mat image;
            std::vector<cv::Mat> channels;
            channels.push_back(channel0);
            channels.push_back(channel1);
            channels.push_back(channel2);
            if (!channel3.empty())
                channels.push_back(channel3);
            cv::merge(channels, image);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// ImageAndMaskCopy
Node *Spawn_ImageOperator_ImageAndMaskCopy(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取遮罩图像");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Mask", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        cv::Mat mask;
        result = get_image(graph, node->Inputs[1], mask);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = mask;

        try_catch_block
        {
            cv::Mat result;
            image.copyTo(result, mask);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// ImageOcrText
Node *Spawn_ImageOperator_OcrText(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "OCR 文本");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs.emplace_back(GetNextId(), PinType::String);
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
            cv::Mat roi = image;
            if (image.channels() == 1)
            {
                cv::cvtColor(image, roi, cv::COLOR_GRAY2BGR);
            }
            if (roi.channels() == 4)
            {
                cv::cvtColor(roi, roi, cv::COLOR_BGRA2BGR);
            }

            auto data = roi.data;
            auto data_size = roi.channels() * roi.cols * roi.rows;
            char result[1024] = {0};
            auto error_code = ocr_image_data(roi.cols, roi.rows, (const char *)roi.data, data_size, result, 1024);
            if (error_code != 0)
                return ExecuteResult::ErrorNode(node->ID, "OCR 失败，错误码：" + std::to_string(error_code));
            std::string text = std::string(result);
            node->Outputs[0].SetValue(text);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

#endif // CHILD_NODES_HPP
