#include "base_nodes.hpp"
#include "../notifiers/Notifier.hpp"
#include <filesystem>

Node *SpawnInputActionNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "InputAction Fire", ImColor(255, 128, 128));
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Delegate);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Pressed", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Released", PinType::Flow);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnBranchNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Branch");
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "False", PinType::Flow);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnDoNNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Do N");
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Enter", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "N", PinType::Int);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Reset", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Exit", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Counter", PinType::Int);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnOutputActionNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "OutputAction");
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Sample", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Event", PinType::Delegate);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnPrintStringNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Print String");
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "In String", PinType::String);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnMessageNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "", ImColor(128, 195, 248));
    auto &node = m_Nodes.back();
    node.Type = NodeType::Simple;
    node.Outputs.emplace_back(GetNextId(), "Message", PinType::String, std::string("Hello, World!"));

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string message;
        if (!node->Outputs[0].GetValue(message))
            message = "Error: Message pin not connected";
        printf("Message: %s\n", message.c_str());
        return true;
    };

    BuildNode(&node);

    return &node;
}

Node *SpawnSetTimerNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Set Timer", ImColor(128, 195, 248));
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Function Name", PinType::Function);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Time", PinType::Float);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Looping", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnLessNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "<", ImColor(128, 195, 248));
    m_Nodes.back().Type = NodeType::Simple;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnWeirdNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "o.O", ImColor(128, 195, 248));
    m_Nodes.back().Type = NodeType::Simple;
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnTraceByChannelNode(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Single Line Trace by Channel", ImColor(255, 128, 64));
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Start", PinType::Flow);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "End", PinType::Int);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Channel", PinType::Float);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Complex", PinType::Bool);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Actors to Ignore", PinType::Int);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Draw Debug Type", PinType::Bool);
    m_Nodes.back().Inputs.emplace_back(GetNextId(), "Ignore Self", PinType::Bool);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Out Hit", PinType::Float);
    m_Nodes.back().Outputs.emplace_back(GetNextId(), "Return Value", PinType::Bool);

    BuildNode(&m_Nodes.back());

    return &m_Nodes.back();
}

Node *SpawnComment(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Test Comment");
    m_Nodes.back().Type = NodeType::Comment;
    m_Nodes.back().Size = ImVec2(300, 200);

    return &m_Nodes.back();
}

Node *Spawn_ImageSource(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)
{
    m_Nodes.emplace_back(GetNextId(), "Image Source");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "Image Path", PinType::String, std::string("C:/"));
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        printf("Executing Image Source\n");
        printf("Inputs: %d\n", (int)node->Inputs.size());
        printf("Outputs: %d\n", (int)node->Outputs.size());
        auto path_pin = node->Inputs[0];
        // 先找到输入端点对应的连线
        auto link = graph->FindPinLink(path_pin.ID);
        if (!link)
            return false;
        // 再找到连线的起始端点，即上一个节点的输出端点
        auto start_pin = graph->FindPin(link->StartPinID);
        // 如果起始端点不存在或者不是输出端点，则返回
        if (!start_pin && start_pin->Kind != PinKind::Output)
            return false;
        // 从上一个节点的输出端点获取数据
        std::string path;
        if (!start_pin->GetValue(path))
            return false;
        printf("Image Path: %s\n", path.c_str());

        std::filesystem::path p(path);
        if (!std::filesystem::exists(p))
            return false;

        std::string ext = p.extension().string();
        if (ext != ".png" && ext != ".jpg" && ext != ".jpeg")
            return false;

        // Load image
        printf("Loading Image\n");
        cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);
        if (image.empty())
            return false;
        node->Outputs[0].Value = image;
        printf("Image Loaded %d %d\n", image.cols, image.rows);
        return true;
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
        printf("Executing Image Source\n");
        printf("Inputs: %d\n", (int)node->Inputs.size());
        printf("Outputs: %d\n", (int)node->Outputs.size());
        auto image_pin = node->Inputs[0];
        auto link = graph->FindPinLink(image_pin.ID);
        if (!link)
            return false;
        auto start_pin = graph->FindPin(link->StartPinID);
        if (!start_pin && start_pin->Kind != PinKind::Output)
            return false;
        cv::Mat image;
        if (!start_pin->Node->Outputs[0].GetValue(image))
            return false;
        printf("Image Input %d %d\n", image.cols, image.rows);

        // Display image
        node->Inputs[0].Value = image;
        return true;
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
        printf("Executing Image Source\n");
        printf("Inputs: %d\n", (int)node->Inputs.size());
        printf("Outputs: %d\n", (int)node->Outputs.size());
        auto image_pin = node->Inputs[0];
        auto link = graph->FindPinLink(image_pin.ID);
        if (!link)
            return false;
        auto start_pin = graph->FindPin(link->StartPinID);
        if (!start_pin && start_pin->Kind != PinKind::Output)
            return false;
        cv::Mat image;
        if (!start_pin->Node->Outputs[0].GetValue(image))
            return false;
        printf("Image Input %d %d\n", image.cols, image.rows);

        // Display image
        node->Inputs[0].Value = image;

        cv::Mat gray;
        if (image.channels() == 1)
            gray = image;
        else if (image.channels() == 3)
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        else if (image.channels() == 4)
            cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
        node->Outputs[0].Value = gray;

        return true;
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
        printf("Executing Image Source\n");
        printf("Inputs: %d\n", (int)node->Inputs.size());
        printf("Outputs: %d\n", (int)node->Outputs.size());
        auto image_pin = node->Inputs[0];
        auto link = graph->FindPinLink(image_pin.ID);
        if (!link)
            return false;
        auto start_pin = graph->FindPin(link->StartPinID);
        if (!start_pin && start_pin->Kind != PinKind::Output)
            return false;
        cv::Mat image;
        if (!start_pin->Node->Outputs[0].GetValue(image))
            return false;
        printf("Image Input %d %d\n", image.cols, image.rows);

        // Display image
        // node->Inputs[0].Value = image;
        float threshold_1 = 50;
        float threshold_2 = 150;
        int size = 3;
        if (!node->Inputs[1].GetValue(threshold_1))
            threshold_1 = 50;
        if (!node->Inputs[2].GetValue(threshold_2))
            threshold_2 = 150;
        if (!node->Inputs[3].GetValue(size))
            size = 3;

        if (size % 2 == 0)
            size += 1;
        cv::Mat canny;
        cv::Canny(image, canny, threshold_1, threshold_2, size);
        node->Outputs[0].Value = canny;

        return true;
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
        printf("Executing Image Source\n");
        printf("Inputs: %d\n", (int)node->Inputs.size());
        printf("Outputs: %d\n", (int)node->Outputs.size());
        auto image_pin = node->Inputs[0];
        auto link = graph->FindPinLink(image_pin.ID);
        if (!link)
            return false;
        auto start_pin = graph->FindPin(link->StartPinID);
        if (!start_pin && start_pin->Kind != PinKind::Output)
            return false;
        cv::Mat image;
        if (!start_pin->Node->Outputs[0].GetValue(image))
            return false;
        printf("Image Input %d %d\n", image.cols, image.rows);

        // Display image
        node->Inputs[0].Value = image;

        cv::Mat bgr;
        if (image.channels() == 3)
            cv::cvtColor(image, bgr, cv::COLOR_RGB2BGR);
        else if (image.channels() == 4)
            cv::cvtColor(image, bgr, cv::COLOR_RGBA2BGRA);
        node->Outputs[0].Value = bgr;

        return true;
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
        printf("Executing Image Source\n");
        printf("Inputs: %d\n", (int)node->Inputs.size());
        printf("Outputs: %d\n", (int)node->Outputs.size());
        auto image_pin = node->Inputs[0];
        auto link = graph->FindPinLink(image_pin.ID);
        if (!link)
            return false;
        auto start_pin = graph->FindPin(link->StartPinID);
        if (!start_pin && start_pin->Kind != PinKind::Output)
            return false;
        cv::Mat image;
        if (!start_pin->Node->Outputs[0].GetValue(image))
            return false;
        printf("Image Input %d %d\n", image.cols, image.rows);

        // Display image
        node->Inputs[0].Value = image;

        cv::Mat rgb;
        if (image.channels() == 3)
            cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
        else if (image.channels() == 4)
            cv::cvtColor(image, rgb, cv::COLOR_BGRA2RGBA);
        node->Outputs[0].Value = rgb;

        return true;
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
                                  {"BGR to RGB", Spawn_ImageOperator_BgrToRgb},
                              }},
        {NodeType::Simple, {
                               {"Message", SpawnMessageNode},
                               {"Less", SpawnLessNode},
                               {"Weird", SpawnWeirdNode},
                           }},
        {NodeType::Comment, {
                                {"Comment", SpawnComment},
                            }},
};