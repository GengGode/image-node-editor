#include "base_nodes.hpp"
#include "child_nodes/child_nodes.hpp"
// #include "../notifiers/Notifier.hpp"
#include <filesystem>

Node *Spawn_ImageFileSource(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像文件源");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像路径", PinType::String, std::string("resources/texture.png"));
    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image, cv::Mat());
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string path;
        auto result = get_value(graph, node->Inputs[0], path);
        if (result.has_error())
            return result;

        std::filesystem::path p(path);
        if (!std::filesystem::exists(p))
            return ExecuteResult::ErrorNode(node->ID, "文件没有找到");

        // std::string ext = p.extension().string();
        // if (ext != ".png" && ext != ".jpg" && ext != ".jpeg")
        //     return ExecuteResult::ErrorNode(node->ID, "不支持的文件格式");

        try_catch_block;
        cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);
        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图片加载失败");
        node->Outputs[0].SetValue(image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

Node *Spawn_ImageViewer(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像查看器");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
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

        try_catch_block
        {
            node->Outputs[0].SetValue(image.channels());
        }
        catch_block_and_return;
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
        try_catch_block
        {
            cv::Mat canny;
            cv::Canny(image, canny, threshold_1, threshold_2, size);
            node->Outputs[0].SetValue(canny);
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

        try_catch_block
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
        catch_block_and_return;
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

std::map<NodeType, NodeWorldGlobal::FactoryGroupFunc_t> NodeWorldGlobal::nodeFactories =
    {
        {NodeType::Blueprint, BlueprintNodes},
        {NodeType::BaseType, BaseTypeNodes},
        {NodeType::BaseConvert, BaseConvertNodes},
        {NodeType::BaseOperation, BaseOperationNodes},
        {NodeType::ImageFlow, {
                                  {"图像文件源", Spawn_ImageFileSource},
                                  {"图像查看器", Spawn_ImageViewer},
                                  {"Image Get Size", Spawn_ImageOperator_ImageGetSize},
                                  {"Image Get Rect", Spawn_ImageOperator_ImageGetRect},
                                  {"Image Get Channels", Spawn_ImageOperator_ImageGetChannels},

                              }},
        {NodeType::ImageType, ImageTypeNodes},
        {NodeType::ImageValue, ImageMathNodes},
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
                                       {"Image ReSize", Spawn_ImageOperator_ImageReSize},
                                       {"Threshold", Spawn_ImageOperator_Threshold},
                                       {"Adaptive Threshold", Spawn_ImageOperator_AdaptiveThreshold},
                                       {"Canny", Spawn_ImageOperator_Canny},
                                       {"Image Get Rect Image", Spawn_ImageOperator_ImageGetRectImage},

                                   }},
        {NodeType::ImageOperation_Edge, ImageOperatorEdgeNodes},
        {NodeType::ImageOther, ImageColorConvertNodes},
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