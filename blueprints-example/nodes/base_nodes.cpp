#include "base_nodes.hpp"
#include "child_nodes/child_nodes.hpp"
#include "../notifiers/Notifier.hpp"
#include <filesystem>

ExecuteResult get_image(Graph *graph, Pin input, cv::Mat &image)
{
    auto link = graph->FindPinLink(input.ID);
    if (!link)
        return ExecuteResult::ErrorPin(input.ID, "Not Find Pin Link");
    auto start_pin = graph->FindPin(link->StartPinID);
    if (!start_pin && start_pin->Kind != PinKind::Output)
        return ExecuteResult::ErrorLink(link->ID, "Not Find Link Start Pin");
    if (!start_pin->GetValue(image))
        return ExecuteResult::ErrorLink(link->ID, "Not Get Value");
    return ExecuteResult::Success();
}
template <typename T>
ExecuteResult get_value(Graph *graph, Pin input, T &value)
{
    auto link = graph->FindPinLink(input.ID);
    if (!link)
    {
        if (!input.GetValue(value))
            return ExecuteResult::ErrorPin(input.ID, std::string("Not Find Pin Link or Not default value type: ") + typeid(T).name());
        return ExecuteResult::Success();
    }
    auto start_pin = graph->FindPin(link->StartPinID);
    if (!start_pin && start_pin->Kind != PinKind::Output)
        return ExecuteResult::ErrorLink(link->ID, "Not Find Link Start Pin");
    if (!start_pin->GetValue(value))
        return ExecuteResult::ErrorLink(link->ID, "Not Get Value");
    return ExecuteResult::Success();
}

Node *SpawnIntToStringNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "", ImColor(128, 195, 248));
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Inputs.emplace_back(GetNextId(), "Int", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Message", PinType::String, std::string("0"));

    node.OnExecute = [](Graph *graph, Node *node)
    {
        int value;
        auto result = get_value(graph, node->Inputs[0], value);
        if (result.has_error())
            return result;
        node->Outputs[0].Value = std::to_string(value);
        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

Node *Spawn_ImageSource(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Image Source");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image Path", PinType::String, std::string("resources/base.jpg"));
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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

        try
        {
            cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);
            if (image.empty())
                return ExecuteResult::ErrorNode(node->ID, "Failed to load image");
            node->Outputs[0].Value = image;
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

Node *Spawn_ImageViewer(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Image Viewer");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        auto result = get_image(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "Empty image");

        node->Inputs[0].Value = image;
        return ExecuteResult::Success();
    };

    BuildNode(&node);

    return &node;
}

Node *Spawn_ImageOperator_Gray(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Gray");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
            node->Outputs[0].Value = gray;
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
Node *Spawn_ImageOperator_Canny(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Canny");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Threshold 1", PinType::Float, 50.0f);
    node.Inputs.emplace_back(GetNextId(), "Threshold 2", PinType::Float, 150.0f);
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Int, 3);

    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
            node->Outputs[0].Value = canny;
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

Node *Spawn_ImageOperator_RgbToBgr(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "RGB to BGR");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
            node->Outputs[0].Value = bgr;
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

Node *Spawn_ImageOperator_RgbaToRgb(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "RGBA to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
            node->Outputs[0].Value = rgb;
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
Node *Spawn_ImageOperator_BgrToRgb(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "BGR to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
            node->Outputs[0].Value = rgb;
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
Node *Spawn_ImageOperator_GrayToRGB(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Gray to RGB");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
            node->Outputs[0].Value = rgb;
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

Node *Spawn_ImageOperator_ImageAddImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Image Add Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
            node->Outputs[0].Value = image;
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

Node *Spawn_ImageOperator_ImageReSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Image ReSize");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Width", PinType::Int, 640);
    node.Inputs.emplace_back(GetNextId(), "Height", PinType::Int, 480);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
            node->Outputs[0].Value = resize;
            return ExecuteResult::Success();
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_ImageGetSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Image Get Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Width", PinType::Int);
    node.Outputs.emplace_back(GetNextId(), "Height", PinType::Int);

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
            node->Outputs[0].Value = image.cols;
            node->Outputs[1].Value = image.rows;
            return ExecuteResult::Success();
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_ImageGetChannels(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Image Get Channels");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Channels", PinType::Int);

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
            node->Outputs[0].Value = image.channels();
            return ExecuteResult::Success();
        }
        catch (const cv::Exception &e)
        {
            return ExecuteResult::ErrorNode(node->ID, e.what());
        }
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_ImageAddInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Image Add Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

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
        node->Inputs[1].Value = value;

        try
        {
            cv::Mat result;
            cv::add(image, value, result);
            node->Outputs[0].Value = result;
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

std::map<NodeType, std::vector<std::pair<std::string, std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)>>>> NodeWorldGlobal::nodeFactories =
    {
        {NodeType::Blueprint, {
                                  {"InputAction", SpawnInputActionNode},
                                  {"Branch", SpawnBranchNode},
                                  {"Do N", SpawnDoNNode},
                                  {"OutputAction", SpawnOutputActionNode},
                                  {"Print String", SpawnPrintStringNode},
                                  {"Message", SpawnMessageNode},
                                  {"Set Timer", SpawnSetTimerNode},
                                  {"Less", SpawnLessNode},
                                  {"Weird", SpawnWeirdNode},
                                  {"Trace by Channel", SpawnTraceByChannelNode},
                              }},
        {NodeType::ImageFlow, {
                                  {"Image Source", Spawn_ImageSource},
                                  {"Image Viewer", Spawn_ImageViewer},
                                  {"Gray", Spawn_ImageOperator_Gray},
                                  {"Canny", Spawn_ImageOperator_Canny},
                                  {"RGB to BGR", Spawn_ImageOperator_RgbToBgr},
                                  {"RGBA to RGB", Spawn_ImageOperator_RgbaToRgb},
                                  {"BGR to RGB", Spawn_ImageOperator_BgrToRgb},
                                  {"Gray to RGB", Spawn_ImageOperator_GrayToRGB},
                                  {"Image Add Image", Spawn_ImageOperator_ImageAddImage},
                                  {"Image ReSize", Spawn_ImageOperator_ImageReSize},
                                  {"Image Get Size", Spawn_ImageOperator_ImageGetSize},
                                  {"Image Get Channels", Spawn_ImageOperator_ImageGetChannels},
                                  {"Image Add Int", Spawn_ImageOperator_ImageAddInt},
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