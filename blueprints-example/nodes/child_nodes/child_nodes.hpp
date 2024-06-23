#ifndef CHILD_NODES_HPP
#define CHILD_NODES_HPP

#include "../base_nodes.hpp"
#include "blueprint/blueprint.hpp"
#include "flow/flow_source.hpp"
#include "maa/maa_task.hpp"
#include "win32/win32_window.hpp"
#include "win32/win32_softinput.hpp"

#include "base/base_type.hpp"
#include "base/base_string.hpp"
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
#include "image/image_operator_match.hpp"
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
        get_value(graph, node->Inputs[0], image);

        try_catch_block;

        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图像为空");
        node->Inputs[0].SetValue(image);

        catch_block_and_return;
    };

    node.ast.code = "cv::imshow(\"Image\", $0);";
    node.ast.add_params({"image"});

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
        get_value(graph, node->Inputs[0], image);

        std::string path;
        get_value(graph, node->Inputs[1], path);

        bool compress;
        get_value(graph, node->Inputs[2], compress);

        int compress_param;
        get_value(graph, node->Inputs[3], compress_param);

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

    node.ast.code = "cv::imwrite($1, $0);";
    node.ast.add_params({"image", "path"});

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
        get_value(graph, node->Inputs[0], image);

        std::string path;
        get_value(graph, node->Inputs[1], path);

        int depth;
        get_value(graph, node->Inputs[2], depth);

        bool little_endian;
        get_value(graph, node->Inputs[3], little_endian);

        std::string header;
        get_value(graph, node->Inputs[4], header);

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

    node.ast.code = "std::ofstream file($1, std::ios::binary);"
                    "if (!file.is_open()) return;"
                    "file.write($4.c_str(), $4.size());"
                    "file.write(reinterpret_cast<char*>($0.data), $0.cols * $0.rows * $0.channels() * static_cast<int>($0.elemSize1()));";
    node.ast.add_params({"image", "path", "depth", "little_endian", "header"});

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
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "宽度");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "高度");

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        try_catch_block
        {
            node->Outputs[0].SetValue(image.size());
            node->Outputs[1].SetValue(image.cols);
            node->Outputs[2].SetValue(image.rows);
        }
        catch_block_and_return;
    };

    node.ast.code = "cv::Size $1 = $0.size();"
                    "int $2 = $0.cols;"
                    "int $3 = $0.rows;";
    node.ast.add_params({"image", "size", "width", "height"});

    BuildNode(&node);
    return &node;
}

// Image Get Rect
Node *Spawn_ImageOperator_ImageGetRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取图像范围");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs.emplace_back(GetNextId(), PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "横坐标");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "纵坐标");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "宽度");
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "高度");

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

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

    node.ast.code = "cv::Rect $1(0, 0, $0.cols, $0.rows);"
                    "int $2 = $1.x;"
                    "int $3 = $1.y;"
                    "int $4 = $1.width;"
                    "int $5 = $1.height;";
    node.ast.add_params({"image", "rect", "x", "y", "width", "height"});

    BuildNode(&node);
    return &node;
}

// Image Get Channels
Node *Spawn_ImageOperator_ImageGetChannels(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取图像通道数");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "通道数");

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        try_catch_block
        {
            node->Outputs[0].SetValue(image.channels());
        }
        catch_block_and_return;
    };

    node.ast.code = "int $1 = $0.channels();";
    node.ast.add_params({"image", "channels"});

    BuildNode(&node);
    return &node;
}

// Image Get Depth
Node *Spawn_ImageOperator_ImageGetDepth(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取图像深度");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs.emplace_back(GetNextId(), PinType::Int, "深度");

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        try_catch_block
        {
            node->Outputs[0].SetValue(image.depth());
        }
        catch_block_and_return;
    };

    node.ast.code = "int $1 = $0.depth();";
    node.ast.add_params({"image", "depth"});

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

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        try_catch_block
        {
            node->Outputs[0].SetValue(image.size());
            node->Outputs[1].SetValue(cv::Point(image.cols / 2, image.rows / 2));
            node->Outputs[2].SetValue(image.channels());
            node->Outputs[3].SetValue(image.depth());
        }
        catch_block_and_return;
    };

    node.ast.code = "cv::Size $1 = $0.size();"
                    "cv::Point $2($0.cols / 2, $0.rows / 2);"
                    "int $3 = $0.channels();"
                    "int $4 = $0.depth();";
    node.ast.add_params({"image", "size", "center", "channels", "depth"});

    BuildNode(&node);
    return &node;
}

// ImageGetRectImage
Node *Spawn_ImageOperator_ImageGetRectImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取范围图像");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        cv::Rect rect;
        get_value(graph, node->Inputs[1], rect);

        try_catch_block
        {
            cv::Mat rect_image = image(rect).clone();
            node->Outputs[0].SetValue(rect_image);
        }
        catch_block_and_return;
    };

    node.ast.code = "cv::Mat $2 = $0($1);";
    node.ast.add_params({"image", "rect", "rect_image"});

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

    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        cv::Rect rect;
        get_value(graph, node->Inputs[1], rect);

        cv::Mat overlay;
        get_value(graph, node->Inputs[2], overlay);

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

    node.ast.code = "cv::Mat $3 = $0.clone();"
                    "$2.copyTo($3($1));";
    node.ast.add_params({"image", "rect", "overlay", "result_image"});

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
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "宽度", 640);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "高度", 480);
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int width = 640;
        get_value(graph, node->Inputs[1], width);

        int height = 480;
        get_value(graph, node->Inputs[2], height);

        try_catch_block
        {
            cv::Mat resize;
            cv::resize(image, resize, cv::Size(width, height));
            node->Outputs[0].SetValue(resize);
        }
        catch_block_and_return;
    };

    node.ast.code = "cv::resize($0, $3, cv::Size($1, $2));";
    node.ast.add_params({"image", "width", "height", "resize"});

    BuildNode(&node);
    return &node;
}

// 遮罩运算
Node *Spawn_ImageOperator_MaskImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Mask Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Mask", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        cv::Mat mask;
        get_value(graph, node->Inputs[1], mask);

        try_catch_block
        {
            cv::Mat masked_image;
            cv::bitwise_and(image, mask, masked_image);
            node->Outputs[0].SetValue(masked_image);
        }
        catch_block_and_return;
    };

    node.ast.code = "cv::bitwise_and($0, $1, $2);";
    node.ast.add_params({"image", "mask", "masked_image"});

    BuildNode(&node);
    return &node;
}

// ImageChannelSplit
Node *Spawn_ImageOperator_ImageChannelSplit(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像通道拆分");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs.emplace_back(GetNextId(), PinType::Image, "通道 1");
    node.Outputs.emplace_back(GetNextId(), PinType::Image, "通道 2");
    node.Outputs.emplace_back(GetNextId(), PinType::Image, "通道 3");
    node.Outputs.emplace_back(GetNextId(), PinType::Image, "通道 4");

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

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

    node.ast.code = "std::vector<cv::Mat> $1;"
                    "cv::Mat $2, $3, $4, $5;"
                    "cv::split($0, $1);"
                    "if ($1.size() > 0) $2 = $1[0];"
                    "if ($1.size() > 1) $3 = $1[1];"
                    "if ($1.size() > 2) $4 = $1[2];"
                    "if ($1.size() > 3) $5 = $1[3];";
    node.ast.add_params({"image", "channels", "channel0", "channel1", "channel2", "channel3"});

    BuildNode(&node);
    return &node;
}

// ImageChannelMerge
Node *Spawn_ImageOperator_ImageChannelMerge(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像通道合并");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "通道 1");
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "通道 2");
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "通道 3");
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "通道 4");
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat channel0;
        get_value(graph, node->Inputs[0], channel0);

        cv::Mat channel1;
        get_value(graph, node->Inputs[1], channel1);

        cv::Mat channel2;
        get_value(graph, node->Inputs[2], channel2);

        cv::Mat channel3;
        get_value(graph, node->Inputs[3], channel3);
        // note: channel3 is optional
        //

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

    node.ast.code = "std::vector<cv::Mat> $1;"
                    "cv::Mat $2;"
                    "$1.push_back($3);"
                    "$1.push_back($4);"
                    "$1.push_back($5);"
                    "if (!$6.empty()) $1.push_back($6);"
                    "cv::merge($1, $2);";
    node.ast.add_params({"_", "channels", "image", "channel0", "channel1", "channel2", "channel3"});

    BuildNode(&node);
    return &node;
}

// ImageAndMaskCopy
Node *Spawn_ImageOperator_ImageAndMaskCopy(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "获取遮罩图像");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "遮罩");
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        cv::Mat mask;
        get_value(graph, node->Inputs[1], mask);

        try_catch_block
        {
            cv::Mat result;
            image.copyTo(result, mask);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    node.ast.code = "$0.copyTo($2, $1);";
    node.ast.add_params({"image", "mask", "result"});

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

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

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

    node.ast.code = "cv::Mat $1 = $0;"
                    "if ($0.channels() == 1) cv::cvtColor($0, $1, cv::COLOR_GRAY2BGR);"
                    "if ($1.channels() == 4) cv::cvtColor($1, $1, cv::COLOR_BGRA2BGR);"
                    "auto $2 = $1.data;"
                    "auto $3 = $1.channels() * $1.cols * $1.rows;"
                    "char $4[1024] = {0};"
                    "auto $5 = ocr_image_data($1.cols, $1.rows, (const char*)$2, $3, $4, 1024);"
                    "if ($5 != 0) return;"
                    "std::string $6 = std::string($4);";
    node.ast.add_params({"image", "roi", "data", "data_size", "result", "error_code", "text"});
    node.ast.global_define = "#include <libocr.h>";

    BuildNode(&node);
    return &node;
}

// image HConcat
Node *Spawn_ImageOperator_HConcatenateImages(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "水平拼接图像");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "左侧图像");
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "右侧图像");
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat left_image;
        get_value(graph, node->Inputs[0], left_image);

        cv::Mat right_image;
        get_value(graph, node->Inputs[1], right_image);

        try_catch_block
        {
            cv::Mat result;
            cv::hconcat(left_image, right_image, result);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    node.ast.code = "cv::Mat $2;"
                    "cv::hconcat($0, $1, $2);";
    node.ast.add_params({"left_image", "right_image", "result"});

    BuildNode(&node);
    return &node;
}

// image VConcat
Node *Spawn_ImageOperator_VConcatenateImages(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "垂直拼接图像");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "上方图像");
    node.Inputs.emplace_back(GetNextId(), PinType::Image, "下方图像");
    node.Outputs.emplace_back(GetNextId(), PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat top_image;
        get_value(graph, node->Inputs[0], top_image);

        cv::Mat bottom_image;
        get_value(graph, node->Inputs[1], bottom_image);

        try_catch_block
        {
            cv::Mat result;
            cv::vconcat(top_image, bottom_image, result);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    node.ast.code = "cv::Mat $2;"
                    "cv::vconcat($0, $1, $2);";
    node.ast.add_params({"top_image", "bottom_image", "result"});

    BuildNode(&node);
    return &node;
}

// image Grid Split
Node *Spawn_ImageOperator_GridSplitImages(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "网格拆分图像");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::Image);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "行数", 2);
    node.Inputs.emplace_back(GetNextId(), PinType::Int, "列数", 2);

    node.Outputs.emplace_back(GetNextId(), PinType::Image, "Image 0, 0");
    node.Outputs.emplace_back(GetNextId(), PinType::Image, "Image 0, 1");
    node.Outputs.emplace_back(GetNextId(), PinType::Image, "Image 1, 0");
    node.Outputs.emplace_back(GetNextId(), PinType::Image, "Image 1, 1");

    node.OnExecute = [](Graph *graph, Node *node)
    {
        static auto SplitImage = [](cv::Mat &image, int rows, int cols, std::vector<cv::Mat> &images)
        {
            int rowsize = image.rows / rows;
            int colsize = image.cols / cols;
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    cv::Mat roi = image(cv::Rect(j * colsize, i * rowsize, colsize, rowsize));
                    images.push_back(roi.clone());
                }
            }
        };

        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int rows = 2;
        get_value(graph, node->Inputs[1], rows);

        int cols = 2;
        get_value(graph, node->Inputs[2], cols);

        try_catch_block
        {
            if (rows <= 0 || cols <= 0)
                return ExecuteResult::ErrorNode(node->ID, "行列数必须大于0");
            if (rows * cols < node->Outputs.size())
            {
                node->Outputs.erase(node->Outputs.begin() + rows * cols, node->Outputs.end());
            }
            if (rows * cols > node->Outputs.size())
            {
                for (int x = 0; x < rows; x++)
                {
                    for (int y = 0; y < cols; y++)
                    {
                        // 跳过已有的输出
                        if (x * cols + y < node->Outputs.size())
                            continue;
                        // 创建新的输出
                        std::string name = "Image " + std::to_string(x) + ", " + std::to_string(y);
                        node->Outputs.emplace_back(graph->get_next_id(), PinType::Image, name);
                        node->Outputs[node->Outputs.size() - 1].app = graph->env.app;
                    }
                }
            }

            std::vector<cv::Mat> images;
            SplitImage(image, rows, cols, images);
            for (size_t i = 0; i < images.size(); i++)
            {
                node->Outputs[i].SetValue(images[i]);
            }
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
static NodeWorldGlobal::FactoryGroupFunc_t ImageBaseNodes = {{"图像查看器", Spawn_ImageViewer},
                                                             {"写入本地文件", Spawn_ImageWriteLocalFile},
                                                             {"写入Raw文件", Spawn_ImageWriteRawFile},
                                                             {"获取图像大小", Spawn_ImageOperator_ImageGetSize},
                                                             {"获取图像范围", Spawn_ImageOperator_ImageGetRect},
                                                             {"获取图像通道数", Spawn_ImageOperator_ImageGetChannels},
                                                             {"获取图像深度", Spawn_ImageOperator_ImageGetDepth},
                                                             {"获取图像信息", Spawn_ImageOperator_ImageGetAllInfo},};
static NodeWorldGlobal::FactoryGroupFunc_t ImageOperationNodes = {
    {"OCR 文本", Spawn_ImageOperator_OcrText},
    {"Mask Image", Spawn_ImageOperator_MaskImage},
    {"图像通道拆分", Spawn_ImageOperator_ImageChannelSplit},
    {"图像通道合并", Spawn_ImageOperator_ImageChannelMerge},
    {"获取遮罩图像", Spawn_ImageOperator_ImageAndMaskCopy},
    {"调整图像大小", Spawn_ImageOperator_ImageReSize},
    {"获取范围图像", Spawn_ImageOperator_ImageGetRectImage},
    {"范围图像覆盖图像", Spawn_ImageOperator_RectImageToImage},
    {"水平拼接图像", Spawn_ImageOperator_HConcatenateImages},
    {"垂直拼接图像", Spawn_ImageOperator_VConcatenateImages},
    {"网格拆分图像", Spawn_ImageOperator_GridSplitImages},
};

static std::vector<std::pair<std::string, factory_func_t>> ImageBaseNodesFactorys = {
    {"图像/图像查看器", Spawn_ImageViewer},
    {"图像/写入/本地文件", Spawn_ImageWriteLocalFile},
    {"图像/写入/Raw文件", Spawn_ImageWriteRawFile},
    {"图像/获取/图像大小", Spawn_ImageOperator_ImageGetSize},
    {"图像/获取/图像范围", Spawn_ImageOperator_ImageGetRect},
    {"图像/获取/图像通道数", Spawn_ImageOperator_ImageGetChannels},
    {"图像/获取/图像深度", Spawn_ImageOperator_ImageGetDepth},
    {"图像/获取/图像信息", Spawn_ImageOperator_ImageGetAllInfo},
};

static std::vector<std::pair<std::string, factory_func_t>> ImageOperationNodesFactorys = {
    {"图像/操作/OCR 文本", Spawn_ImageOperator_OcrText},
    {"图像/操作/Mask Image", Spawn_ImageOperator_MaskImage},
    {"图像/操作/图像通道拆分", Spawn_ImageOperator_ImageChannelSplit},
    {"图像/操作/图像通道合并", Spawn_ImageOperator_ImageChannelMerge},
    {"图像/操作/获取遮罩图像", Spawn_ImageOperator_ImageAndMaskCopy},
    {"图像/操作/调整图像大小", Spawn_ImageOperator_ImageReSize},
    {"图像/操作/获取范围图像", Spawn_ImageOperator_ImageGetRectImage},
    {"图像/操作/范围图像覆盖图像", Spawn_ImageOperator_RectImageToImage},
    {"图像/操作/水平拼接图像", Spawn_ImageOperator_HConcatenateImages},
    {"图像/操作/垂直拼接图像", Spawn_ImageOperator_VConcatenateImages},
    {"图像/操作/网格拆分图像", Spawn_ImageOperator_GridSplitImages},
};

#endif // CHILD_NODES_HPP
