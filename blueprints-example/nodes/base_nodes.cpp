#include "base_nodes.hpp"
#include "child_nodes/child_nodes.hpp"
// #include "../notifiers/Notifier.hpp"
#include <filesystem>

Node *Spawn_ImageFileSource(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像文件源");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::String, "图像路径", std::string("resources/texture.png"));
    node.Outputs.emplace_back(GetNextId(), PinType::Image);
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
                                  {"获取图像大小", Spawn_ImageOperator_ImageGetSize},
                                  {"获取图像范围", Spawn_ImageOperator_ImageGetRect},
                                  {"获取图像通道数", Spawn_ImageOperator_ImageGetChannels},
                              }},
        {NodeType::ImageType, ImageTypeNodes},
        {NodeType::ImageValue, ImageMathNodes},
        {NodeType::ImageDraw, ImageDrawNodes},
        {NodeType::ImageSource, ImageSourceNodes},
        {NodeType::ImageOperation, {
                                       {"Mask Image", Spawn_ImageOperator_MaskImage},
                                       {"图像通道拆分", Spawn_ImageOperator_ImageChannelSplit},
                                       {"Image Channel Merge", Spawn_ImageOperator_ImageChannelMerge},
                                       {"Image And Mask Copy", Spawn_ImageOperator_ImageAndMaskCopy},
                                       {"Image ReSize", Spawn_ImageOperator_ImageReSize},
                                       {"Image Get Rect Image", Spawn_ImageOperator_ImageGetRectImage},
                                   }},
        {NodeType::ImageOperation_Threshold, ImageOperatorThresholdNodes},
        {NodeType::ImageOperation_Morphology, ImageOperatorMorphologyNodes},
        {NodeType::ImageOperation_Filter, ImageOperatorFilterNodes},
        {NodeType::ImageOperation_Edge, ImageOperatorEdgeNodes},
        {NodeType::ImageOperation_Feature, ImageFeatureNodes},
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