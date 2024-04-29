#include "base_nodes.hpp"
#include "child_nodes/child_nodes.hpp"
// #include "../notifiers/Notifier.hpp"
#include <filesystem>

Node *Spawn_ImageSource(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Source");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image Path", PinType::String, std::string("resources/texture.png"));
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image, cv::Mat());
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string path;
        auto result = get_value(graph, node->Inputs[0], path);
        if (result.has_error())
            return result;

        std::filesystem::path p(path);
        if (!std::filesystem::exists(p))
            return ExecuteResult::ErrorNode(node->ID, "File not found");

        std::string ext = p.extension().string();
        if (ext != ".png" && ext != ".jpg" && ext != ".jpeg")
            return ExecuteResult::ErrorNode(node->ID, "Invalid file format");

        try_catch_block;
        cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);
        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "Failed to load image");
        node->Outputs[0].SetValue(image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

Node *Spawn_ImageViewer(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Viewer");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "Empty image");

        node->Inputs[0].SetValue(image);
        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

// threshold
Node *Spawn_ImageOperator_Threshold(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Threshold");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Threshold", PinType::Int, 128);
    node.Inputs.emplace_back(GetNextId(), "Max Value", PinType::Int, 255);
    node.Inputs.emplace_back(GetNextId(), "Type", PinType::Int, cv::THRESH_BINARY);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int threshold = 128;
        result = get_value(graph, node->Inputs[1], threshold);
        if (result.has_error())
            return result;

        int max_value = 255;
        result = get_value(graph, node->Inputs[2], max_value);
        if (result.has_error())
            return result;

        int type = cv::THRESH_BINARY;
        result = get_value(graph, node->Inputs[3], type);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = image;
        node->Inputs[1].Value = threshold;
        node->Inputs[2].Value = max_value;
        node->Inputs[3].Value = type;

        try_catch_block;
        cv::Mat thresholded;
        cv::threshold(image, thresholded, threshold, max_value, type);
        node->Outputs[0].SetValue(thresholded);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// adaptive threshold
Node *Spawn_ImageOperator_AdaptiveThreshold(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Adaptive Threshold");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Max Value", PinType::Int, 255);
    node.Inputs.emplace_back(GetNextId(), "Type", PinType::Int, cv::ADAPTIVE_THRESH_MEAN_C);
    node.Inputs.emplace_back(GetNextId(), "Block Size", PinType::Int, 11);
    node.Inputs.emplace_back(GetNextId(), "C", PinType::Int, 2);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int max_value = 255;
        result = get_value(graph, node->Inputs[1], max_value);
        if (result.has_error())
            return result;

        int type = cv::ADAPTIVE_THRESH_MEAN_C;
        result = get_value(graph, node->Inputs[2], type);
        if (result.has_error())
            return result;

        int block_size = 11;
        result = get_value(graph, node->Inputs[3], block_size);
        if (result.has_error())
            return result;

        int c = 2;
        result = get_value(graph, node->Inputs[4], c);
        if (result.has_error())
            return result;

        node->Inputs[0].Value = image;
        node->Inputs[1].Value = max_value;
        node->Inputs[2].Value = type;
        node->Inputs[3].Value = block_size;
        node->Inputs[4].Value = c;

        try_catch_block;
        cv::Mat thresholded;
        cv::adaptiveThreshold(image, thresholded, max_value, type, cv::THRESH_BINARY, block_size, c);
        node->Outputs[0].SetValue(thresholded);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// canny
Node *Spawn_ImageOperator_Canny(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Canny");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
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
        result = get_value(graph, node->Inputs[1], threshold_1);
        if (result.has_error())
            return result;

        float threshold_2 = 150;
        result = get_value(graph, node->Inputs[2], threshold_2);
        if (result.has_error())
            return result;

        int size = 3;
        result = get_value(graph, node->Inputs[3], size);
        if (result.has_error())
            return result;

        if (size % 2 == 0)
            return ExecuteResult::ErrorNode(node->ID, "Size must be odd");
        try
        {
            cv::Mat canny;
            cv::Canny(image, canny, threshold_1, threshold_2, size);
            node->Outputs[0].SetValue(canny);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

/* *** */
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

        try
        {
            cv::Mat bgr;
            if (image.channels() == 3)
                cv::cvtColor(image, bgr, cv::COLOR_RGB2BGR);
            else if (image.channels() == 4)
                cv::cvtColor(image, bgr, cv::COLOR_RGBA2BGRA);
            node->Outputs[0].SetValue(bgr);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat rgb;
            cv::cvtColor(image, rgb, cv::COLOR_RGBA2RGB);
            node->Outputs[0].SetValue(rgb);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat rgb;
            if (image.channels() == 3)
                cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
            else if (image.channels() == 4)
                cv::cvtColor(image, rgb, cv::COLOR_BGRA2RGBA);
            node->Outputs[0].SetValue(rgb);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat rgb;
            if (image.channels() == 1)
                cv::cvtColor(image, rgb, cv::COLOR_GRAY2RGB);
            node->Outputs[0].SetValue(rgb);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
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
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

/* *** */
// image add image
Node *Spawn_ImageOperator_ImageAddImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Add Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        auto result = get_image(graph, node->Inputs[0], image_right);
        if (result.has_error())
            return result;

        cv::Mat image_left;
        result = get_image(graph, node->Inputs[1], image_left);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image_right;
        node->Inputs[1].Value = image_left;

        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");

        try
        {
            cv::Mat image;
            cv::add(image_right, image_left, image);
            node->Outputs[0].SetValue(image);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}
// image subtract image
Node *Spawn_ImageOperator_ImageSubImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "Image Sub Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        auto result = get_image(graph, node->Inputs[0], image_right);
        if (result.has_error())
            return result;

        cv::Mat image_left;
        result = get_image(graph, node->Inputs[1], image_left);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image_right;
        node->Inputs[1].Value = image_left;

        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");

        try_catch_block;
        cv::Mat image;
        cv::subtract(image_right, image_left, image);
        node->Outputs[0].SetValue(image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image multiply image
Node *Spawn_ImageOperator_ImageMulImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "Image Mul Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        auto result = get_image(graph, node->Inputs[0], image_right);
        if (result.has_error())
            return result;

        cv::Mat image_left;
        result = get_image(graph, node->Inputs[1], image_left);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image_right;
        node->Inputs[1].Value = image_left;

        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");

        try_catch_block;
        cv::Mat image;
        cv::multiply(image_right, image_left, image);
        node->Outputs[0].SetValue(image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image divide image
Node *Spawn_ImageOperator_ImageDivImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Div Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        auto result = get_image(graph, node->Inputs[0], image_right);
        if (result.has_error())
            return result;

        cv::Mat image_left;
        result = get_image(graph, node->Inputs[1], image_left);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image_right;
        node->Inputs[1].Value = image_left;

        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");

        try_catch_block;
        cv::Mat image;
        cv::divide(image_right, image_left, image);
        node->Outputs[0].SetValue(image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

Node *Spawn_ImageOperator_ImageReSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image ReSize");
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

        try
        {
            cv::Mat resize;
            cv::resize(image, resize, cv::Size(width, height));
            node->Outputs[0].SetValue(resize);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// Image Get Size
Node *Spawn_ImageOperator_ImageGetSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Get Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Width", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Height", PinType::Int);

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

        try
        {
            node->Outputs[0].SetValue(image.size());
            node->Outputs[1].SetValue(image.cols);
            node->Outputs[2].SetValue(image.rows);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// Image Get Rect
Node *Spawn_ImageOperator_ImageGetRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Get Rect");
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

        try
        {
            cv::Rect rect(0, 0, image.cols, image.rows);
            node->Outputs[0].SetValue(rect);
            node->Outputs[1].SetValue(rect.x);
            node->Outputs[2].SetValue(rect.y);
            node->Outputs[3].SetValue(rect.width);
            node->Outputs[4].SetValue(rect.height);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// Image Get Channels
Node *Spawn_ImageOperator_ImageGetChannels(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Get Channels");
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

        try
        {
            node->Outputs[0].SetValue(image.channels());
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_IntToSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int to Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Width", PinType::Int, 1024);
    node.Inputs.emplace_back(GetNextId(), "Height", PinType::Int, 1024);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        int width = 1024;
        auto result = get_value(graph, node->Inputs[0], width);
        if (result.has_error())
            return result;

        int height = 1024;
        result = get_value(graph, node->Inputs[1], height);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = width;
        node->Inputs[1].Value = height;

        try
        {
            cv::Size size(width, height);
            node->Outputs[0].SetValue(size);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_SizeToInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size to Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Width", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Height", PinType::Int);

    node.Outputs[0].app = app;
    node.Outputs[1].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        auto result = get_value(graph, node->Inputs[0], size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = size;

        try
        {
            node->Outputs[0].SetValue(size.width);
            node->Outputs[1].SetValue(size.height);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_IntToPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int to Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "X", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Y", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        int x = 0;
        auto result = get_value(graph, node->Inputs[0], x);
        if (result.has_error())
            return result;

        int y = 0;
        result = get_value(graph, node->Inputs[1], y);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = x;
        node->Inputs[1].Value = y;

        try
        {
            cv::Point point(x, y);
            node->Outputs[0].SetValue(point);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_PointToInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point to Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "X", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Y", PinType::Int);

    node.Outputs[0].app = app;
    node.Outputs[1].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        auto result = get_value(graph, node->Inputs[0], point);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point;

        try
        {
            node->Outputs[0].SetValue(point.x);
            node->Outputs[1].SetValue(point.y);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_IntToRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int to Rect");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "X", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Y", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Width", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "Height", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        int x = 0;
        auto result = get_value(graph, node->Inputs[0], x);
        if (result.has_error())
            return result;

        int y = 0;
        result = get_value(graph, node->Inputs[1], y);
        if (result.has_error())
            return result;

        int width = 0;
        result = get_value(graph, node->Inputs[2], width);
        if (result.has_error())
            return result;

        int height = 0;
        result = get_value(graph, node->Inputs[3], height);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = x;
        node->Inputs[1].Value = y;
        node->Inputs[2].Value = width;
        node->Inputs[3].Value = height;

        try
        {
            cv::Rect rect(x, y, width, height);
            node->Outputs[0].SetValue(rect);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_RectToInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect to Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), "X", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Y", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Width", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Height", PinType::Int);

    node.Outputs[0].app = app;
    node.Outputs[1].app = app;
    node.Outputs[2].app = app;
    node.Outputs[3].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        auto result = get_value(graph, node->Inputs[0], rect);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = rect;

        try
        {
            node->Outputs[0].SetValue(rect.x);
            node->Outputs[1].SetValue(rect.y);
            node->Outputs[2].SetValue(rect.width);
            node->Outputs[3].SetValue(rect.height);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// PointAndSizeToRect
Node *Spawn_ImageOperator_PointAndSizeToRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point and Size to Rect");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        auto result = get_value(graph, node->Inputs[0], point);
        if (result.has_error())
            return result;

        cv::Size size;
        result = get_value(graph, node->Inputs[1], size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point;
        node->Inputs[1].Value = size;

        try
        {
            cv::Rect rect(point, size);
            node->Outputs[0].SetValue(rect);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// RectToPointAndSize
Node *Spawn_ImageOperator_RectToPointAndSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect to Point and Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;
    node.Outputs[1].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        auto result = get_value(graph, node->Inputs[0], rect);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = rect;

        try
        {
            cv::Point point(rect.x, rect.y);
            cv::Size size(rect.width, rect.height);
            node->Outputs[0].SetValue(point);
            node->Outputs[1].SetValue(size);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// PointToSize
Node *Spawn_ImageOperator_PointToSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point to Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        auto result = get_value(graph, node->Inputs[0], point);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point;

        try
        {
            cv::Size size(point.x, point.y);
            node->Outputs[0].SetValue(size);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// SizeToPoint
Node *Spawn_ImageOperator_SizeToPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size to Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        auto result = get_value(graph, node->Inputs[0], size);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = size;

        try
        {
            cv::Point point(size.width, size.height);
            node->Outputs[0].SetValue(point);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// SizeAddSize
Node *Spawn_ImageOperator_SizeAddSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Add Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Size right", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Size left", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size_right;
        auto result = get_value(graph, node->Inputs[0], size_right);
        if (result.has_error())
            return result;

        cv::Size size_left;
        result = get_value(graph, node->Inputs[1], size_left);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = size_right;
        node->Inputs[1].Value = size_left;

        try
        {
            cv::Size size(size_right.width + size_left.width, size_right.height + size_left.height);
            node->Outputs[0].SetValue(size);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// SizeSubSize
Node *Spawn_ImageOperator_SizeSubSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Sub Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Size right", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Size left", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size_right;
        auto result = get_value(graph, node->Inputs[0], size_right);
        if (result.has_error())
            return result;

        cv::Size size_left;
        result = get_value(graph, node->Inputs[1], size_left);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = size_right;
        node->Inputs[1].Value = size_left;

        try
        {
            cv::Size size(size_right.width - size_left.width, size_right.height - size_left.height);
            node->Outputs[0].SetValue(size);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// SizeMulInt
Node *Spawn_ImageOperator_SizeMulInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Mul Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        auto result = get_value(graph, node->Inputs[0], size);
        if (result.has_error())
            return result;

        int value = 1;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = size;
        node->Inputs[1].Value = value;

        try
        {
            cv::Size result(size.width * value, size.height * value);
            node->Outputs[0].SetValue(result);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// SizeDivInt
Node *Spawn_ImageOperator_SizeDivInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Div Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        auto result = get_value(graph, node->Inputs[0], size);
        if (result.has_error())
            return result;

        int value = 1;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = size;
        node->Inputs[1].Value = value;

        try
        {
            cv::Size result(size.width / value, size.height / value);
            node->Outputs[0].SetValue(result);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// SizeMulFloat
Node *Spawn_ImageOperator_SizeMulFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Mul Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        auto result = get_value(graph, node->Inputs[0], size);
        if (result.has_error())
            return result;

        float value = 1.0f;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = size;
        node->Inputs[1].Value = value;

        try
        {
            cv::Size result(cvRound(size.width * value), cvRound(size.height * value));
            node->Outputs[0].SetValue(result);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// SizeDivFloat
Node *Spawn_ImageOperator_SizeDivFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Div Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        auto result = get_value(graph, node->Inputs[0], size);
        if (result.has_error())
            return result;

        float value = 1.0f;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = size;
        node->Inputs[1].Value = value;

        try
        {
            cv::Size result(cvRound(size.width / value), cvRound(size.height / value));
            node->Outputs[0].SetValue(result);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// PointAddPoint
Node *Spawn_ImageOperator_PointAddPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Add Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point right", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Point left", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point_right;
        auto result = get_value(graph, node->Inputs[0], point_right);
        if (result.has_error())
            return result;

        cv::Point point_left;
        result = get_value(graph, node->Inputs[1], point_left);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point_right;
        node->Inputs[1].Value = point_left;

        try
        {
            cv::Point point(point_right.x + point_left.x, point_right.y + point_left.y);
            node->Outputs[0].SetValue(point);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// PointSubPoint
Node *Spawn_ImageOperator_PointSubPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Sub Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point right", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Point left", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point_right;
        auto result = get_value(graph, node->Inputs[0], point_right);
        if (result.has_error())
            return result;

        cv::Point point_left;
        result = get_value(graph, node->Inputs[1], point_left);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point_right;
        node->Inputs[1].Value = point_left;

        try
        {
            cv::Point point(point_right.x - point_left.x, point_right.y - point_left.y);
            node->Outputs[0].SetValue(point);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// PointMulInt
Node *Spawn_ImageOperator_PointMulInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Mul Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        auto result = get_value(graph, node->Inputs[0], point);
        if (result.has_error())
            return result;

        int value = 1;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point;
        node->Inputs[1].Value = value;

        try
        {
            cv::Point result(point.x * value, point.y * value);
            node->Outputs[0].SetValue(result);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// PointDivInt
Node *Spawn_ImageOperator_PointDivInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Div Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        auto result = get_value(graph, node->Inputs[0], point);
        if (result.has_error())
            return result;

        int value = 1;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point;
        node->Inputs[1].Value = value;

        try
        {
            cv::Point result(point.x / value, point.y / value);
            node->Outputs[0].SetValue(result);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// PointMulFloat
Node *Spawn_ImageOperator_PointMulFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Mul Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        auto result = get_value(graph, node->Inputs[0], point);
        if (result.has_error())
            return result;

        float value = 1.0f;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point;
        node->Inputs[1].Value = value;

        try
        {
            cv::Point result(cvRound(point.x * value), cvRound(point.y * value));
            node->Outputs[0].SetValue(result);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// PointDivFloat
Node *Spawn_ImageOperator_PointDivFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Div Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        auto result = get_value(graph, node->Inputs[0], point);
        if (result.has_error())
            return result;

        float value = 1.0f;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = point;
        node->Inputs[1].Value = value;

        try
        {
            cv::Point result(cvRound(point.x / value), cvRound(point.y / value));
            node->Outputs[0].SetValue(result);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// ImageGetRectImage
Node *Spawn_ImageOperator_ImageGetRectImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Get Rect Image");
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
        result = get_value(graph, node->Inputs[1], rect);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = rect;

        try
        {
            cv::Mat rect_image = image(rect).clone();
            node->Outputs[0].SetValue(rect_image);
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_ImageAddInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Add Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;

        int value = 0;
        result = get_value(graph, node->Inputs[1], value);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;

        try
        {
            cv::Mat result;
            result = image + cv::Scalar(value, value, value, value);
            node->Outputs[0].SetValue(result);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

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

        try
        {
            cv::Mat dilate;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::dilate(image, dilate, element);
            node->Outputs[0].SetValue(dilate);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat erode;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::erode(image, erode, element);
            node->Outputs[0].SetValue(erode);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat open;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, open, cv::MORPH_OPEN, element);
            node->Outputs[0].SetValue(open);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat close;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, close, cv::MORPH_CLOSE, element);
            node->Outputs[0].SetValue(close);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat gradient;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, gradient, cv::MORPH_GRADIENT, element);
            node->Outputs[0].SetValue(gradient);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat top_hat;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, top_hat, cv::MORPH_TOPHAT, element);
            node->Outputs[0].SetValue(top_hat);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat black_hat;
            cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
            cv::morphologyEx(image, black_hat, cv::MORPH_BLACKHAT, element);
            node->Outputs[0].SetValue(black_hat);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
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

        try
        {
            cv::Mat masked_image;
            cv::bitwise_and(image, mask, masked_image);
            node->Outputs[0].SetValue(masked_image);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

// ImageChannelSplit
Node *Spawn_ImageOperator_ImageChannelSplit(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Channel Split");
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

        try
        {
            std::vector<cv::Mat> channels;
            cv::split(image, channels);
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
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

// ImageChannelMerge
Node *Spawn_ImageOperator_ImageChannelMerge(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Channel Merge");
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

        try
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
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }
        return ExecuteResult::Success();
    };

    BuildNode(&node);
    return &node;
}

// ImageAndMaskCopy
Node *Spawn_ImageOperator_ImageAndMaskCopy(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image And Mask Copy");
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

        try
        {
            cv::Mat result;
            image.copyTo(result, mask);
            node->Outputs[0].SetValue(result);
        }
        catch (const std::exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
        catch (...)
        {
            return ExecuteResult::ErrorNode(node->ID, "Unknown error");
        }

        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

std::map<NodeType, std::vector<std::pair<std::string, std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)>>>> NodeWorldGlobal::nodeFactories =
    {
        {NodeType::Blueprint, {
                                  //{"InputAction", SpawnInputActionNode},
                                  //{"Branch", SpawnBranchNode},
                                  //{"Do N", SpawnDoNNode},
                                  //{"OutputAction", SpawnOutputActionNode},
                                  //{"Print String", SpawnPrintStringNode},
                                  //{"Message", SpawnMessageNode},
                                  //{"Set Timer", SpawnSetTimerNode},
                                  //{"Less", SpawnLessNode},
                                  //{"Weird", SpawnWeirdNode},
                                  //{"Trace by Channel", SpawnTraceByChannelNode},
                              }},
        {NodeType::BaseType, BaseTypeNodes},
        {NodeType::BaseConvert, BaseConvertNodes},
        {NodeType::BaseOperation, BaseOperationNodes},
        {NodeType::ImageFlow, {
                                  {"Image Source", Spawn_ImageSource},
                                  {"Image Viewer", Spawn_ImageViewer},
                                  {"Image Get Size", Spawn_ImageOperator_ImageGetSize},
                                  {"Image Get Rect", Spawn_ImageOperator_ImageGetRect},
                                  {"Image Get Channels", Spawn_ImageOperator_ImageGetChannels},

                              }},
        {NodeType::ImageValue, {
                                   {"Int to Size", Spawn_ImageOperator_IntToSize},
                                   {"Size to Int", Spawn_ImageOperator_SizeToInt},
                                   {"Int to Point", Spawn_ImageOperator_IntToPoint},
                                   {"Point to Int", Spawn_ImageOperator_PointToInt},
                                   {"Int to Rect", Spawn_ImageOperator_IntToRect},
                                   {"Rect to Int", Spawn_ImageOperator_RectToInt},
                                   {"Point and Size to Rect", Spawn_ImageOperator_PointAndSizeToRect},
                                   {"Rect to Point and Size", Spawn_ImageOperator_RectToPointAndSize},
                                   {"Point to Size", Spawn_ImageOperator_PointToSize},
                                   {"Size to Point", Spawn_ImageOperator_SizeToPoint},
                                   {"Point Add Point", Spawn_ImageOperator_PointAddPoint},
                                   {"Point Sub Point", Spawn_ImageOperator_PointSubPoint},
                                   {"Point Mul Int", Spawn_ImageOperator_PointMulInt},
                                   {"Point Div Int", Spawn_ImageOperator_PointDivInt},
                                   {"Point Mul Float", Spawn_ImageOperator_PointMulFloat},
                                   {"Point Div Float", Spawn_ImageOperator_PointDivFloat},
                                   {"Size Add Size", Spawn_ImageOperator_SizeAddSize},
                                   {"Size Sub Size", Spawn_ImageOperator_SizeSubSize},
                                   {"Size Mul Int", Spawn_ImageOperator_SizeMulInt},
                                   {"Size Div Int", Spawn_ImageOperator_SizeDivInt},
                                   {"Size Mul Float", Spawn_ImageOperator_SizeMulFloat},
                                   {"Size Div Float", Spawn_ImageOperator_SizeDivFloat},
                               }},
        {NodeType::ImageOperation, {
                                       {"Dilate", Spawn_ImageOperator_Dilate},
                                       {"Erode", Spawn_ImageOperator_Erode},
                                       {"Morphology Open", Spawn_ImageOperator_MorphologyOpen},
                                       {"Morphology Close", Spawn_ImageOperator_MorphologyClose},
                                       {"Morphology Gradient", Spawn_ImageOperator_MorphologyGradient},
                                       {"Morphology Top Hat", Spawn_ImageOperator_MorphologyTopHat},
                                       {"Morphology Black Hat", Spawn_ImageOperator_MorphologyBlackHat},
                                       {"Mask Image", Spawn_ImageOperator_MaskImage},
                                       {"Image Channel Split", Spawn_ImageOperator_ImageChannelSplit},
                                       {"Image Channel Merge", Spawn_ImageOperator_ImageChannelMerge},
                                       {"Image And Mask Copy", Spawn_ImageOperator_ImageAndMaskCopy},
                                       {"Image Add Int", Spawn_ImageOperator_ImageAddInt},
                                       {"Image Add Image", Spawn_ImageOperator_ImageAddImage},
                                       {"Image Sub Image", Spawn_ImageOperator_ImageSubImage},
                                       {"Image Mul Image", Spawn_ImageOperator_ImageMulImage},
                                       {"Image Div Image", Spawn_ImageOperator_ImageDivImage},
                                       {"Image ReSize", Spawn_ImageOperator_ImageReSize},
                                       {"Threshold", Spawn_ImageOperator_Threshold},
                                       {"Adaptive Threshold", Spawn_ImageOperator_AdaptiveThreshold},
                                       {"Canny", Spawn_ImageOperator_Canny},
                                       {"Image Get Rect Image", Spawn_ImageOperator_ImageGetRectImage},

                                   }},
        {NodeType::ImageOther, {
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

                               }},
        {NodeType::Simple, {
                               {"Int to String", SpawnIntToStringNode},
                               {"Message", SpawnMessageNode},
                               {"Less", SpawnLessNode},
                               {"Weird", SpawnWeirdNode},
                           }},
        {NodeType::Comment, {
                                {"Comment", SpawnComment},
                            }},
};