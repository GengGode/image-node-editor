#pragma once
#include "base_nodes.hpp"

// PointAddPoint
Node *Spawn_ImageOperator_PointAddPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Add Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Point right", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Point left", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point_right;
        get_value(graph, node->Inputs[0], point_right);

        cv::Point point_left;
        get_value(graph, node->Inputs[1], point_left);

        try_catch_block
        {
            cv::Point point(point_right.x + point_left.x, point_right.y + point_left.y);
            node->Outputs[0].SetValue(point);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// PointSubPoint
Node *Spawn_ImageOperator_PointSubPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Sub Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Point right", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Point left", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point_right;
        get_value(graph, node->Inputs[0], point_right);

        cv::Point point_left;
        get_value(graph, node->Inputs[1], point_left);

        try_catch_block
        {
            cv::Point point(point_right.x - point_left.x, point_right.y - point_left.y);
            node->Outputs[0].SetValue(point);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// PointMulInt
Node *Spawn_ImageOperator_PointMulInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Mul Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        get_value(graph, node->Inputs[0], point);

        int value = 1;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Point result(point.x * value, point.y * value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// PointDivInt
Node *Spawn_ImageOperator_PointDivInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Div Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        get_value(graph, node->Inputs[0], point);

        int value = 1;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Point result(point.x / value, point.y / value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// PointMulFloat
Node *Spawn_ImageOperator_PointMulFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Mul Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        get_value(graph, node->Inputs[0], point);

        float value = 1.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Point result(cvRound(point.x * value), cvRound(point.y * value));
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// PointDivFloat
Node *Spawn_ImageOperator_PointDivFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Point Div Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Point", PinType::Point);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Point point;
        get_value(graph, node->Inputs[0], point);

        float value = 1.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Point result(cvRound(point.x / value), cvRound(point.y / value));
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// SizeAddSize
Node *Spawn_ImageOperator_SizeAddSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Add Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Size right", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Size left", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size_right;
        get_value(graph, node->Inputs[0], size_right);

        cv::Size size_left;
        get_value(graph, node->Inputs[1], size_left);

        try_catch_block
        {
            cv::Size size(size_right.width + size_left.width, size_right.height + size_left.height);
            node->Outputs[0].SetValue(size);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// SizeSubSize
Node *Spawn_ImageOperator_SizeSubSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Sub Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Size right", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Size left", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size_right;
        get_value(graph, node->Inputs[0], size_right);

        cv::Size size_left;
        get_value(graph, node->Inputs[1], size_left);

        try_catch_block
        {
            cv::Size size(size_right.width - size_left.width, size_right.height - size_left.height);
            node->Outputs[0].SetValue(size);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// SizeMulInt
Node *Spawn_ImageOperator_SizeMulInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Mul Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        get_value(graph, node->Inputs[0], size);

        int value = 1;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Size result(size.width * value, size.height * value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// SizeDivInt
Node *Spawn_ImageOperator_SizeDivInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Div Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        get_value(graph, node->Inputs[0], size);

        int value = 1;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Size result(size.width / value, size.height / value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// SizeMulFloat
Node *Spawn_ImageOperator_SizeMulFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Mul Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        get_value(graph, node->Inputs[0], size);

        float value = 1.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Size result(cvRound(size.width * value), cvRound(size.height * value));
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// SizeDivFloat
Node *Spawn_ImageOperator_SizeDivFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Size Div Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Size", PinType::Size);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Size size;
        get_value(graph, node->Inputs[0], size);
        float value = 1.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Size result(cvRound(size.width / value), cvRound(size.height / value));
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// RectAddPoint
Node *Spawn_ImageOperator_RectAddPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Add Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);

        cv::Point point;
        get_value(graph, node->Inputs[1], point);

        try_catch_block
        {
            cv::Rect result(rect.x + point.x, rect.y + point.y, rect.width, rect.height);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectSubPoint
Node *Spawn_ImageOperator_RectSubPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Sub Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);

        cv::Point point;
        get_value(graph, node->Inputs[1], point);

        try_catch_block
        {
            cv::Rect result(rect.x - point.x, rect.y - point.y, rect.width, rect.height);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectAddSize
Node *Spawn_ImageOperator_RectAddSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Add Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);
        cv::Size size;
        get_value(graph, node->Inputs[1], size);

        try_catch_block
        {
            cv::Rect result(rect.x, rect.y, rect.width + size.width, rect.height + size.height);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectSubSize
Node *Spawn_ImageOperator_RectSubSize(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Sub Size");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Size", PinType::Size);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);
        cv::Size size;
        get_value(graph, node->Inputs[1], size);

        try_catch_block
        {
            cv::Rect result(rect.x, rect.y, rect.width - size.width, rect.height - size.height);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// RectMulInt
Node *Spawn_ImageOperator_RectMulInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Mul Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);

        int value = 1;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Rect result(rect.x * value, rect.y * value, rect.width * value, rect.height * value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectDivInt
Node *Spawn_ImageOperator_RectDivInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Div Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);
        int value = 1;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Rect result(rect.x / value, rect.y / value, rect.width / value, rect.height / value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectMulFloat
Node *Spawn_ImageOperator_RectMulFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Mul Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);
        float value = 1.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Rect result(cvRound(rect.x * value), cvRound(rect.y * value), cvRound(rect.width * value), cvRound(rect.height * value));
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectDivFloat
Node *Spawn_ImageOperator_RectDivFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Div Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);

        float value = 1.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Rect result(cvRound(rect.x / value), cvRound(rect.y / value), cvRound(rect.width / value), cvRound(rect.height / value));
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectAndRect
Node *Spawn_ImageOperator_RectAndRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect And Rect");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect right", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Rect left", PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect_right;
        get_value(graph, node->Inputs[0], rect_right);

        cv::Rect rect_left;
        get_value(graph, node->Inputs[1], rect_left);

        try_catch_block
        {
            cv::Rect result(rect_right & rect_left);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectOrRect
Node *Spawn_ImageOperator_RectOrRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Or Rect");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect right", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Rect left", PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), "Rect", PinType::Rect);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect_right;
        get_value(graph, node->Inputs[0], rect_right);
        cv::Rect rect_left;
        get_value(graph, node->Inputs[1], rect_left);
        try_catch_block
        {
            cv::Rect result(rect_right | rect_left);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectIsContainPoint
Node *Spawn_ImageOperator_RectIsContainPoint(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Is Contain Point");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Point", PinType::Point);
    node.Outputs.emplace_back(GetNextId(), "Result", PinType::Bool);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect;
        get_value(graph, node->Inputs[0], rect);

        cv::Point point;
        get_value(graph, node->Inputs[1], point);

        try_catch_block
        {
            bool result = rect.contains(point);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectIsContainRect
Node *Spawn_ImageOperator_RectIsContainRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Is Contain Rect");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect right", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Rect left", PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), "Result", PinType::Bool);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect_right;
        get_value(graph, node->Inputs[0], rect_right);
        cv::Rect rect_left;
        get_value(graph, node->Inputs[1], rect_left);

        try_catch_block
        {
            bool result = rect_right.contains(rect_left.tl()) && rect_right.contains(rect_left.br());
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}
// RectIsIntersectRect
Node *Spawn_ImageOperator_RectIsIntersectRect(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Rect Is Intersect Rect");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Rect right", PinType::Rect);
    node.Inputs.emplace_back(GetNextId(), "Rect left", PinType::Rect);
    node.Outputs.emplace_back(GetNextId(), "Result", PinType::Bool);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Rect rect_right;
        get_value(graph, node->Inputs[0], rect_right);

        cv::Rect rect_left;
        get_value(graph, node->Inputs[1], rect_left);

        try_catch_block
        {
            bool result = (rect_right & rect_left).area() > 0;
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// image add int
Node *Spawn_ImageOperator_ImageAddInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Add Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int value = 0;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Mat result;
            result = image + cv::Scalar(value, value, value, value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image sub int
Node *Spawn_ImageOperator_ImageSubInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Sub Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 0);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int value = 0;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Mat result;
            result = image - cv::Scalar(value, value, value, value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image mul int
Node *Spawn_ImageOperator_ImageMulInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Mul Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int value = 1;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Mat result;
            result = image * value;
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image div int
Node *Spawn_ImageOperator_ImageDivInt(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Div Int");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Int, 1);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int value = 1;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Mat result;
            result = image / value;
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image add float
Node *Spawn_ImageOperator_ImageAddFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Add Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        float value = 0.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Mat result;
            result = image + cv::Scalar(value, value, value, value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image sub float
Node *Spawn_ImageOperator_ImageSubFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Sub Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 0.0f);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        float value = 0.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Mat result;
            result = image - cv::Scalar(value, value, value, value);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image mul float
Node *Spawn_ImageOperator_ImageMulFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Mul Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        float value = 1.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Mat result;
            result = image * value;
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image div float
Node *Spawn_ImageOperator_ImageDivFloat(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Div Float");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Value", PinType::Float, 1.0f);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        float value = 1.0f;
        get_value(graph, node->Inputs[1], value);

        try_catch_block
        {
            cv::Mat result;
            result = image / value;
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// image add image
Node *Spawn_ImageOperator_ImageAddImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "Image Add Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        get_value(graph, node->Inputs[0], image_right);

        cv::Mat image_left;
        get_value(graph, node->Inputs[1], image_left);

        try_catch_block
        {
            if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
                return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
            if (image_right.channels() != image_left.channels())
                return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");
            cv::Mat result;
            cv::add(image_right, image_left, result);
            node->Outputs[0].SetValue(result);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image subtract image
Node *Spawn_ImageOperator_ImageSubImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "Image Sub Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        get_value(graph, node->Inputs[0], image_right);

        cv::Mat image_left;
        get_value(graph, node->Inputs[1], image_left);

        try_catch_block;
        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");
        cv::Mat result;
        cv::subtract(image_right, image_left, result);
        node->Outputs[0].SetValue(result);
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
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        get_value(graph, node->Inputs[0], image_right);

        cv::Mat image_left;
        get_value(graph, node->Inputs[1], image_left);

        try_catch_block;
        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");
        cv::Mat result;
        cv::multiply(image_right, image_left, result);
        node->Outputs[0].SetValue(result);
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
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        get_value(graph, node->Inputs[0], image_right);

        cv::Mat image_left;
        get_value(graph, node->Inputs[1], image_left);

        try_catch_block;
        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");
        cv::Mat result;
        cv::divide(image_right, image_left, result);
        node->Outputs[0].SetValue(result);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// image and image
Node *Spawn_ImageOperator_ImageAndImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "Image And Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        get_value(graph, node->Inputs[0], image_right);

        cv::Mat image_left;
        get_value(graph, node->Inputs[1], image_left);

        try_catch_block;

        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");
        cv::Mat result;
        cv::bitwise_and(image_right, image_left, result);
        node->Outputs[0].SetValue(result);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image or image
Node *Spawn_ImageOperator_ImageOrImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "Image Or Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        get_value(graph, node->Inputs[0], image_right);

        cv::Mat image_left;
        get_value(graph, node->Inputs[1], image_left);

        try_catch_block;
        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");
        cv::Mat result;
        cv::bitwise_or(image_right, image_left, result);
        node->Outputs[0].SetValue(result);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image xor image
Node *Spawn_ImageOperator_ImageXorImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "Image Xor Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        get_value(graph, node->Inputs[0], image_right);

        cv::Mat image_left;
        get_value(graph, node->Inputs[1], image_left);

        try_catch_block;
        if (image_right.cols != image_left.cols || image_right.rows != image_left.rows)
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same size");
        if (image_right.channels() != image_left.channels())
            return ExecuteResult::ErrorNode(node->ID, "Images must have the same number of channels");
        cv::Mat result;
        cv::bitwise_xor(image_right, image_left, result);
        node->Outputs[0].SetValue(result);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}
// image not
Node *Spawn_ImageOperator_ImageNotImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "Image Not Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Image", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        try_catch_block;
        cv::Mat result;
        cv::bitwise_not(image, result);
        node->Outputs[0].SetValue(result);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// image is equal
Node *Spawn_ImageOperator_ImageIsEqualImage(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{

    m_Nodes.emplace_back(GetNextId(), "Image Is Equal Image");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageValue;
    node.Inputs.emplace_back(GetNextId(), "Image right", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "Image left", PinType::Image);
    node.Outputs.emplace_back(GetNextId(), "Result", PinType::Bool);

    node.OnExecute = [](Graph *graph, Node *node)
    {
        cv::Mat image_right;
        get_value(graph, node->Inputs[0], image_right);

        cv::Mat image_left;
        get_value(graph, node->Inputs[1], image_left);

        try_catch_block;
        bool result = cv::countNonZero(image_right != image_left) == 0;
        node->Outputs[0].SetValue(result);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageMathNodes = {
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
    {"Rect And Rect", Spawn_ImageOperator_RectAndRect},
    {"Rect Or Rect", Spawn_ImageOperator_RectOrRect},
    {"Rect Is Contain Point", Spawn_ImageOperator_RectIsContainPoint},
    {"Rect Is Contain Rect", Spawn_ImageOperator_RectIsContainRect},
    {"Rect Is Intersect Rect", Spawn_ImageOperator_RectIsIntersectRect},
    {"Image Add Int", Spawn_ImageOperator_ImageAddInt},
    {"Image Sub Int", Spawn_ImageOperator_ImageSubInt},
    {"Image Mul Int", Spawn_ImageOperator_ImageMulInt},
    {"Image Div Int", Spawn_ImageOperator_ImageDivInt},
    {"Image Add Float", Spawn_ImageOperator_ImageAddFloat},
    {"Image Sub Float", Spawn_ImageOperator_ImageSubFloat},
    {"Image Mul Float", Spawn_ImageOperator_ImageMulFloat},
    {"Image Div Float", Spawn_ImageOperator_ImageDivFloat},
    {"Image Add Image", Spawn_ImageOperator_ImageAddImage},
    {"Image Sub Image", Spawn_ImageOperator_ImageSubImage},
    {"Image Mul Image", Spawn_ImageOperator_ImageMulImage},
    {"Image Div Image", Spawn_ImageOperator_ImageDivImage},
    {"Image And Image", Spawn_ImageOperator_ImageAndImage},
    {"Image Or Image", Spawn_ImageOperator_ImageOrImage},
    {"Image Xor Image", Spawn_ImageOperator_ImageXorImage},
    {"Image Not Image", Spawn_ImageOperator_ImageNotImage},
    {"Image Is Equal Image", Spawn_ImageOperator_ImageIsEqualImage},
};

static std::vector<std::pair<std::string, factory_func_t>> ImageMathNodesFactorys = {
    {"图像/运算/Point/Add Point", Spawn_ImageOperator_PointAddPoint},
    {"图像/运算/Point/Sub Point", Spawn_ImageOperator_PointSubPoint},
    {"图像/运算/Point/Mul Int", Spawn_ImageOperator_PointMulInt},
    {"图像/运算/Point/Div Int", Spawn_ImageOperator_PointDivInt},
    {"图像/运算/Point/Mul Float", Spawn_ImageOperator_PointMulFloat},
    {"图像/运算/Point/Div Float", Spawn_ImageOperator_PointDivFloat},
    {"图像/运算/Size/Add Size", Spawn_ImageOperator_SizeAddSize},
    {"图像/运算/Size/Sub Size", Spawn_ImageOperator_SizeSubSize},
    {"图像/运算/Size/Mul Int", Spawn_ImageOperator_SizeMulInt},
    {"图像/运算/Size/Div Int", Spawn_ImageOperator_SizeDivInt},
    {"图像/运算/Size/Mul Float", Spawn_ImageOperator_SizeMulFloat},
    {"图像/运算/Size/Div Float", Spawn_ImageOperator_SizeDivFloat},
    {"图像/运算/Rect/And Rect", Spawn_ImageOperator_RectAndRect},
    {"图像/运算/Rect/Or Rect", Spawn_ImageOperator_RectOrRect},
    {"图像/运算/Rect/Is Contain Point", Spawn_ImageOperator_RectIsContainPoint},
    {"图像/运算/Rect/Is Contain Rect", Spawn_ImageOperator_RectIsContainRect},
    {"图像/运算/Rect/Is Intersect Rect", Spawn_ImageOperator_RectIsIntersectRect},
    {"图像/运算/Image/Add Int", Spawn_ImageOperator_ImageAddInt},
    {"图像/运算/Image/Sub Int", Spawn_ImageOperator_ImageSubInt},
    {"图像/运算/Image/Mul Int", Spawn_ImageOperator_ImageMulInt},
    {"图像/运算/Image/Div Int", Spawn_ImageOperator_ImageDivInt},
    {"图像/运算/Image/Add Float", Spawn_ImageOperator_ImageAddFloat},
    {"图像/运算/Image/Sub Float", Spawn_ImageOperator_ImageSubFloat},
    {"图像/运算/Image/Mul Float", Spawn_ImageOperator_ImageMulFloat},
    {"图像/运算/Image/Div Float", Spawn_ImageOperator_ImageDivFloat},
    {"图像/运算/Image/Add Image", Spawn_ImageOperator_ImageAddImage},
    {"图像/运算/Image/Sub Image", Spawn_ImageOperator_ImageSubImage},
    {"图像/运算/Image/Mul Image", Spawn_ImageOperator_ImageMulImage},
    {"图像/运算/Image/Div Image", Spawn_ImageOperator_ImageDivImage},
    {"图像/运算/Image/And Image", Spawn_ImageOperator_ImageAndImage},
    {"图像/运算/Image/Or Image", Spawn_ImageOperator_ImageOrImage},
    {"图像/运算/Image/Xor Image", Spawn_ImageOperator_ImageXorImage},
    {"图像/运算/Image/Not Image", Spawn_ImageOperator_ImageNotImage},
    {"图像/运算/Image/Is Equal Image", Spawn_ImageOperator_ImageIsEqualImage},
};
