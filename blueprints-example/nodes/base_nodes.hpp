#ifndef BASE_NODE_HPP
#define BASE_NODE_HPP

#include "../utilities/builders.h"
#include "../utilities/widgets.h"
#include "../notifiers/Notifier.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <application.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <optional>

#include <opencv2/opencv.hpp>

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

enum class PinType
{
    Flow,
    Image,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
    Delegate,
};

typedef std::variant<int, float, bool, std::string, cv::Mat> PinValue;

enum class PinKind
{
    Output,
    Input
};

enum class NodeType
{
    Blueprint,
    ImageFlow,
    Simple,
    Comment
};
static std::vector<std::pair<std::string, NodeType>> nodeTypes = {
    {"Blueprint", NodeType::Blueprint},
    {"ImageFlow", NodeType::ImageFlow},
    {"Simple", NodeType::Simple},
    {"Comment", NodeType::Comment}};

struct Node;
struct Link;

struct Pin
{
    ed::PinId ID;
    ::Node *Node;
    std::string Name;
    PinType Type;
    PinValue Value;
    PinKind Kind;
    bool NeedInputSource = false;
    bool IsConnected;
    bool HoldImageTexture;
    void *ImageTexture = nullptr;

    Pin(int id, const char *name, PinType type, PinValue value = PinValue()) : ID(id), Node(nullptr), Name(name), Type(type), Value(value), Kind(PinKind::Input)
    {
    }

    const std::map<std::size_t, PinType> typeMap = {
        {typeid(int).hash_code(), PinType::Int},
        {typeid(float).hash_code(), PinType::Float},
        {typeid(bool).hash_code(), PinType::Bool},
        {typeid(std::string).hash_code(), PinType::String},
        {typeid(cv::Mat).hash_code(), PinType::Image},
    };
    template <typename T>
    bool GetValue(T &value)
    {
        if (typeMap.at(typeid(T).hash_code()) == Type && std::holds_alternative<T>(Value))
        {
            value = std::get<T>(Value);
            return true;
        }
        return false;
    }

    template <typename T>
    bool SetValue(T value)
    {
        if (typeMap.at(typeid(T).hash_code()) == Type)
        {
            Value = value;
            return true;
        }
        return false;
    }
    bool HasImage()
    {
        return Type == PinType::Image && std::holds_alternative<cv::Mat>(Value) && !std::get<cv::Mat>(Value).empty();
    }

    bool GenImageTexture(Application *app)
    {
        if (Type == PinType::Image && std::holds_alternative<cv::Mat>(Value))
        {
            cv::Mat image = std::get<cv::Mat>(Value);
            if (image.empty())
            {
                return false;
            }
            if (image.empty())
                return false;
            if (image.channels() == 1)
                cv::cvtColor(image, image, cv::COLOR_GRAY2RGBA);
            else if (image.channels() == 3)
                cv::cvtColor(image, image, cv::COLOR_RGB2RGBA);
            int width = image.cols;
            int height = image.rows;
            ImageTexture = app->CreateTexture(image.data, width, height);
            return true;
        }
        return false;
    }
};

struct Graph;

struct ErrorInfo
{
    using SourceId = std::variant<ed::NodeId, ed::PinId, ed::LinkId, std::string>;
    SourceId Source;
    std::string Message;
    ErrorInfo(SourceId source, const std::string &message) : Source(source), Message(message) {}
};
struct ExecuteResult
{
    std::optional<ErrorInfo> Error;

    ExecuteResult(std::optional<ErrorInfo> error = std::nullopt) : Error(error) {}

    explicit operator bool() const { return Error.has_value(); }
    bool has_error() const { return Error.has_value(); }

    static ExecuteResult Success() { return ExecuteResult(); }
    static ExecuteResult ErrorNode(ed::NodeId nodeId, const std::string &message) { return ExecuteResult(ErrorInfo(nodeId, message)); }
    static ExecuteResult ErrorPin(ed::PinId pinId, const std::string &message) { return ExecuteResult(ErrorInfo(pinId, message)); }
    static ExecuteResult ErrorLink(ed::LinkId linkId, const std::string &message) { return ExecuteResult(ErrorInfo(linkId, message)); }
    static ExecuteResult ErrorCustom(const std::string &message) { return ExecuteResult(ErrorInfo(std::string(), message)); }
};
struct Node
{
    ed::NodeId ID;
    std::string Name;
    std::vector<Pin> Inputs;
    std::vector<Pin> Outputs;
    ImColor Color;
    NodeType Type;
    ImVec2 Size;

    std::string State;
    std::string SavedState;

    Node(int id, const char *name, ImColor color = ImColor(255, 255, 255)) : ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
    {
    }
    std::function<ExecuteResult(Graph *, Node *)> OnExecute = [](Graph *, Node *)
    {
        return ExecuteResult::ErrorCustom("Null Impl");
    };
};

struct Link
{
    ed::LinkId ID;

    ed::PinId StartPinID;
    ed::PinId EndPinID;

    ImColor Color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId) : ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
    {
    }
};

struct Graph
{
    std::vector<Node> Nodes;
    std::vector<Link> Links;

    Node *FindNode(ed::NodeId id)
    {
        for (auto &node : Nodes)
            if (node.ID == id)
                return &node;

        return nullptr;
    }

    Link *FindLink(ed::LinkId id)
    {
        for (auto &link : Links)
            if (link.ID == id)
                return &link;

        return nullptr;
    }

    Pin *FindPin(ed::PinId id)
    {
        if (!id)
            return nullptr;

        for (auto &node : Nodes)
        {
            for (auto &pin : node.Inputs)
                if (pin.ID == id)
                    return &pin;

            for (auto &pin : node.Outputs)
                if (pin.ID == id)
                    return &pin;
        }

        return nullptr;
    }

    bool IsPinLinked(ed::PinId id)
    {
        if (!id)
            return false;

        for (auto &link : Links)
            if (link.StartPinID == id || link.EndPinID == id)
                return true;

        return false;
    }

    Link *FindPinLink(ed::PinId id)
    {
        if (!id)
            return nullptr;

        for (auto &link : Links)
            if (link.StartPinID == id || link.EndPinID == id)
                return &link;

        return nullptr;
    }

    std::vector<Link *> FindPinLinks(ed::PinId id)
    {
        std::vector<Link *> links;

        for (auto &link : Links)
            if (link.StartPinID == id || link.EndPinID == id)
                links.push_back(&link);

        return links;
    }
};
struct NodeIdLess
{
    bool operator()(const ed::NodeId &lhs, const ed::NodeId &rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

struct NodeWorldGlobal
{
    static std::map<NodeType, std::vector<std::pair<std::string, std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes)>>>> nodeFactories;
};

#endif // BASE_NODE_HPP