#pragma once
#include "base_nodes.hpp"

Node *Spawn_ImageOperator_IntToPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int to Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            cv::Point point(x, y);
            node->Outputs[0].SetValue(point);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_IntToSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int to Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            cv::Size size(width, height);
            node->Outputs[0].SetValue(size);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_IntToRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int to Rect");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            cv::Rect rect(x, y, width, height);
            node->Outputs[0].SetValue(rect);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_PointToInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point to Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            node->Outputs[0].SetValue(point.x);
            node->Outputs[1].SetValue(point.y);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// PointToSize
Node *Spawn_ImageOperator_PointToSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point to Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            cv::Size size(point.x, point.y);
            node->Outputs[0].SetValue(size);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_SizeToInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size to Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            node->Outputs[0].SetValue(size.width);
            node->Outputs[1].SetValue(size.height);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// SizeToPoint
Node *Spawn_ImageOperator_SizeToPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size to Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            cv::Point point(size.width, size.height);
            node->Outputs[0].SetValue(point);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

Node *Spawn_ImageOperator_RectToInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect to Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            node->Outputs[0].SetValue(rect.x);
            node->Outputs[1].SetValue(rect.y);
            node->Outputs[2].SetValue(rect.width);
            node->Outputs[3].SetValue(rect.height);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectToPointAndSize
Node *Spawn_ImageOperator_RectToPointAndSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect to Point and Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            cv::Point point(rect.x, rect.y);
            cv::Size size(rect.width, rect.height);
            node->Outputs[0].SetValue(point);
            node->Outputs[1].SetValue(size);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// PointAndSizeToRect
Node *Spawn_ImageOperator_PointAndSizeToRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point and Size to Rect");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
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

        try_catch_block
        {
            cv::Rect rect(point, size);
            node->Outputs[0].SetValue(rect);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// cv::Scalar -1 randomColor
Node *Spawn_ImageOperator_RandomColor(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "随机颜色");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
    node.Outputs.emplace_back(GetNextId(), "颜色", PinType::Color);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        // Display image

        try_catch_block
        {
            cv::Scalar color = cv::Scalar::all(-1);
            node->Outputs[0].SetValue(color);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// cv::Scalar(r, g, b, a) int to color
Node *Spawn_ImageOperator_IntToColor(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Int to Color");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
    node.Inputs.emplace_back(GetNextId(), "R", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "G", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "B", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "A", PinType::Int, 255);
    node.Outputs.emplace_back(GetNextId(), "颜色", PinType::Color);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        int r = 0;
        auto result = get_value(graph, node->Inputs[0], r);
        if (result.has_error())
            return result;

        int g = 0;
        result = get_value(graph, node->Inputs[1], g);
        if (result.has_error())
            return result;

        int b = 0;
        result = get_value(graph, node->Inputs[2], b);
        if (result.has_error())
            return result;

        int a = 255;
        result = get_value(graph, node->Inputs[3], a);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = r;
        node->Inputs[1].Value = g;
        node->Inputs[2].Value = b;
        node->Inputs[3].Value = a;

        try_catch_block
        {
            cv::Scalar color(r, g, b, a);
            node->Outputs[0].SetValue(color);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// cv::Mat createMat(int rows, int cols, int type, const cv::Scalar &s)
Node *Spawn_ImageOperator_CreateImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "创建图片");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageType;
    node.Inputs.emplace_back(GetNextId(), "Rows", PinType::Int, 1024);
    node.Inputs.emplace_back(GetNextId(), "Cols", PinType::Int, 1024);
    node.Inputs.emplace_back(GetNextId(), "通道数", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "颜色", PinType::Color, cv::Scalar(0, 0, 0, 0));
    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        int rows = 1024;
        get_value(graph, node->Inputs[0], rows);

        int cols = 1024;
        get_value(graph, node->Inputs[1], cols);

        int channels = 3;
        get_value(graph, node->Inputs[2], channels);

        cv::Scalar scalar(0, 0, 0, 0);
        get_value(graph, node->Inputs[3], scalar);

        // Display image
        node->Inputs[0].Value = rows;
        node->Inputs[1].Value = cols;
        node->Inputs[2].Value = channels;
        node->Inputs[3].Value = scalar;

        try_catch_block
        {
            cv::Mat image(rows, cols, CV_MAKETYPE(CV_8U, channels), scalar);
            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageTypeNodes = {
    {"Int to Point", Spawn_ImageOperator_IntToPoint},
    {"Int to Size", Spawn_ImageOperator_IntToSize},
    {"Int to Rect", Spawn_ImageOperator_IntToRect},
    {"Point to Int", Spawn_ImageOperator_PointToInt},
    {"Point to Size", Spawn_ImageOperator_PointToSize},
    {"Size to Int", Spawn_ImageOperator_SizeToInt},
    {"Size to Point", Spawn_ImageOperator_SizeToPoint},
    {"Rect to Int", Spawn_ImageOperator_RectToInt},
    {"Rect to Point and Size", Spawn_ImageOperator_RectToPointAndSize},
    {"Point and Size to Rect", Spawn_ImageOperator_PointAndSizeToRect},
    {"随机颜色", Spawn_ImageOperator_RandomColor},
    {"Int to Color", Spawn_ImageOperator_IntToColor},
    {"创建图片", Spawn_ImageOperator_CreateImage},
};
