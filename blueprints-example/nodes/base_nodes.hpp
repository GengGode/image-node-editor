#ifndef BASE_NODE_HPP
#define BASE_NODE_HPP

#include "../utilities/builders.h"
#include "../utilities/widgets.h"

#include "node_port_types.hpp"

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
#include <atomic>
#include <future>

#include <opencv2/opencv.hpp>

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

enum class NodeType
{
    Blueprint,
    BaseType,
    BaseConvert,
    BaseOperation,
    ImageFlow,
    ImageType,
    ImageValue,
    ImageDraw,
    ImageSource, // 图像源
    ImageOperation,
    ImageOperation_Threshold,  // 阈值类
    ImageOperation_Filter,     // 滤波类
    ImageOperation_Morphology, // 形态学类
    ImageOperation_Edge,       // 边缘类
    ImageOperation_Histogram,  // 直方图类
    ImageOperation_Feature,    // 特征类
    ImageOperation_Match,      // 匹配类
    ImageOperation_Other,      // 其他类
    ImageOther,
    Simple,
    Comment
};
static std::vector<std::pair<std::string, NodeType>> nodeTypes = {
    {"蓝图", NodeType::Blueprint},
    {"基础类型", NodeType::BaseType},
    {"基础转换", NodeType::BaseConvert},
    {"基础操作", NodeType::BaseOperation},
    {"图像流程", NodeType::ImageFlow},
    {"图像类型", NodeType::ImageType},
    {"图像值", NodeType::ImageValue},
    {"图像绘制", NodeType::ImageDraw},
    {"图像源", NodeType::ImageSource},
    {"图像操作", NodeType::ImageOperation},
    {"图像操作-阈值类", NodeType::ImageOperation_Threshold},
    {"图像操作-滤波类", NodeType::ImageOperation_Filter},
    {"图像操作-形态学类", NodeType::ImageOperation_Morphology},
    {"图像操作-边缘类", NodeType::ImageOperation_Edge},
    {"图像操作-直方图类", NodeType::ImageOperation_Histogram},
    {"图像操作-特征类", NodeType::ImageOperation_Feature},
    {"图像操作-匹配类", NodeType::ImageOperation_Match},
    {"图像操作-其他类", NodeType::ImageOperation_Other},
    {"图像其他操作", NodeType::ImageOther},
    {"Simple", NodeType::Simple},
    {"Comment", NodeType::Comment}};

struct Node;
struct Link;

struct NodeWorldGlobal
{
    using NodeFactory_t = std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)>;
    using FactoryGroupFunc_t = std::vector<std::pair<std::string, NodeFactory_t>>;
    static std::map<NodeType, FactoryGroupFunc_t> nodeFactories;
    inline static std::thread::id main_thread_id = std::this_thread::get_id();
};

struct Pin
{
    ed::PinId ID;
    ::Node *Node;
    std::string Name;
    PinType Type;
    port_value_t Value;
    PinKind Kind;
    bool NeedInputSource = false;
    bool IsConnected;
    bool HoldImageTexture;
    void *ImageTexture = nullptr;
    bool needUpdateTexture = false;
    Application *app;
    void event_value_changed()
    {
        if (std::this_thread::get_id() != NodeWorldGlobal::main_thread_id)
            return;
        if (Type == PinType::Image && app)
        {
            cv::Mat image = std::get<cv::Mat>(Value);
            if (image.empty())
                return;
            if (image.channels() == 1)
                cv::cvtColor(image, image, cv::COLOR_GRAY2RGBA);
            else if (image.channels() == 3)
                cv::cvtColor(image, image, cv::COLOR_RGB2RGBA);
            int width = image.cols;
            int height = image.rows;
            if (ImageTexture)
            {
                app->DestroyTexture(ImageTexture);
            }
            ImageTexture = app->CreateTexture(image.data, width, height);
        }
    };

    bool can_execute()
    {
        if (Kind == PinKind::Input && Value.valueless_by_exception())
        {
            return false;
        }
        return true;
    }

    Pin(int id, const char *name, PinType type, port_value_t value = port_value_t()) : ID(id), Node(nullptr), Name(name), Type(type), Value(value), Kind(PinKind::Input)
    {
    }
    Pin(int id, PinType type, std::string name = "", port_value_t value = port_value_t()) : ID(id), Node(nullptr), Name(name), Type(type), Value(value), Kind(PinKind::Input)
    {
        if (name.empty())
            Name = typeLabelNames.at(type);
    }

    template <typename T>
    bool GetValue(T &value)
    {
        if (typeMap.at(typeid(T).hash_code()) == Type && std::holds_alternative<T>(Value))
        {
            value = std::get<T>(Value);
            if (std::holds_alternative<cv::Mat>(Value))
            {
                if (needUpdateTexture)
                {
                    event_value_changed();
                    needUpdateTexture = false;
                }
            }
            return true;
        }
        return false;
    }

    template <typename T>
    bool SetValue(T value)
    {
        if (typeMap.at(typeid(T).hash_code()) == Type)
        {
            if (std::holds_alternative<T>(Value) == false)
            {
                Value = value;
                needUpdateTexture = true;
                event_value_changed();
                return true;
            }
            bool isChanged = !is_equal(std::get<T>(Value), value);
            Value = value;
            if (isChanged)
            {
                needUpdateTexture = true;
                event_value_changed();
            }
            return true;
        }
        return false;
    }

    template <typename T, class Pred>
    bool SetValue(T value, Pred pred)
    {
        if (typeMap.at(typeid(T).hash_code()) == Type)
        {
            if (std::holds_alternative<T>(Value) == false)
            {
                Value = value;
                needUpdateTexture = true;
                event_value_changed();
                return true;
            }
            bool isChanged = !is_equal(std::get<T>(Value), value);
            Value = value;
            if (isChanged)
            {
                needUpdateTexture = true;
                event_value_changed();
                pred();
            }
            return true;
        }
        return false;
    }

    bool HasImage()
    {
        if (Type != PinType::Image)
            return false;
        if (std::holds_alternative<cv::Mat>(Value) == false)
            return false;
        if (std::get<cv::Mat>(Value).empty())
            return false;
        if (needUpdateTexture)
        {
            event_value_changed();
            needUpdateTexture = false;
        }
        return true;
    }
};

struct Graph;

struct ErrorInfo
{
    using SourceId = std::variant<ed::NodeId, ed::PinId, ed::LinkId /*, std::string*/>;
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
    bool has_node_error() const { return has_error() && std::holds_alternative<ed::NodeId>(Error->Source); }
    bool has_pin_error() const { return has_error() && std::holds_alternative<ed::PinId>(Error->Source); }
    bool has_link_error() const { return has_error() && std::holds_alternative<ed::LinkId>(Error->Source); }
    bool equal_source(ed::NodeId nodeId) const { return has_node_error() && std::get<ed::NodeId>(Error->Source) == nodeId; }
    bool equal_source(ed::PinId pinId) const { return has_pin_error() && std::get<ed::PinId>(Error->Source) == pinId; }
    bool equal_source(ed::LinkId linkId) const { return has_link_error() && std::get<ed::LinkId>(Error->Source) == linkId; }

    static ExecuteResult Success() { return ExecuteResult(); }
    static ExecuteResult ErrorNode(ed::NodeId nodeId, const std::string &message) { return ExecuteResult(ErrorInfo(nodeId, message)); }
    static ExecuteResult ErrorPin(ed::PinId pinId, const std::string &message) { return ExecuteResult(ErrorInfo(pinId, message)); }
    static ExecuteResult ErrorLink(ed::LinkId linkId, const std::string &message) { return ExecuteResult(ErrorInfo(linkId, message)); }
    // static ExecuteResult ErrorCustom(const std::string &message) { return ExecuteResult(ErrorInfo(std::string(), message)); }
};

struct node_ui
{
    // 节点是否折叠为小图标
    bool is_expanded = true;
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
    ImVec2 Position;

    std::string State;
    std::string SavedState;

    std::function<ExecuteResult(Graph *, Node *)> OnExecute = [](Graph *, Node *node)
    {
        return ExecuteResult::ErrorNode(node->ID, "Null Impl");
    };

    std::function<ExecuteResult(Graph *, Node *)> OnExecuteEx = [](Graph *graph, Node *node)
    {
        node->RunningThreadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        node->BeginExecuteTime = std::chrono::steady_clock::now();
        node->IsRunning = true;
        auto result = node->OnExecute(graph, node);
        node->IsRunning = false;
        node->EndExecuteTime = std::chrono::steady_clock::now();
        node->ExecuteTime = std::chrono::duration_cast<std::chrono::steady_clock::duration>(*node->EndExecuteTime - *node->BeginExecuteTime);
        return result;
    };

    ExecuteResult LastExecuteResult;
    std::optional<std::chrono::steady_clock::time_point> BeginExecuteTime;
    std::optional<std::chrono::steady_clock::time_point> EndExecuteTime;
    std::optional<std::chrono::steady_clock::duration> ExecuteTime;
    std::atomic<bool> IsRunning = false;
    std::atomic<size_t> RunningThreadId = 0;

    node_ui ui;

    Node(int id, const char *name, ImColor color = ImColor(255, 255, 255)) : ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
    {
    }
    // 三五法则
    Node(const Node &node)
    {
        ID = node.ID;
        Name = node.Name;
        Inputs = node.Inputs;
        Outputs = node.Outputs;
        Color = node.Color;
        Type = node.Type;
        Size = node.Size;
        Position = node.Position;
        State = node.State;
        SavedState = node.SavedState;
        OnExecute = node.OnExecute;
        LastExecuteResult = node.LastExecuteResult;
        BeginExecuteTime = node.BeginExecuteTime;
        EndExecuteTime = node.EndExecuteTime;
        ExecuteTime = node.ExecuteTime;
        IsRunning.store(node.IsRunning.load());
        RunningThreadId.store(node.RunningThreadId.load());
        ui = node.ui;
    }
    Node &operator=(const Node &node)
    {
        if (this != &node)
        {
            ID = node.ID;
            Name = node.Name;
            Inputs = node.Inputs;
            Outputs = node.Outputs;
            Color = node.Color;
            Type = node.Type;
            Size = node.Size;
            Position = node.Position;
            State = node.State;
            SavedState = node.SavedState;
            OnExecute = node.OnExecute;
            LastExecuteResult = node.LastExecuteResult;
            BeginExecuteTime = node.BeginExecuteTime;
            EndExecuteTime = node.EndExecuteTime;
            ExecuteTime = node.ExecuteTime;
            IsRunning.store(node.IsRunning.load());
            RunningThreadId.store(node.RunningThreadId.load());
            ui = node.ui;
        }
        return *this;
    }
    Node(Node &&node)
    {
        ID = node.ID;
        Name = std::move(node.Name);
        Inputs = std::move(node.Inputs);
        Outputs = std::move(node.Outputs);
        Color = node.Color;
        Type = node.Type;
        Size = node.Size;
        Position = node.Position;
        State = std::move(node.State);
        SavedState = std::move(node.SavedState);
        OnExecute = node.OnExecute;
        LastExecuteResult = node.LastExecuteResult;
        BeginExecuteTime = node.BeginExecuteTime;
        EndExecuteTime = node.EndExecuteTime;
        ExecuteTime = node.ExecuteTime;
        IsRunning.store(node.IsRunning.load());
        RunningThreadId.store(node.RunningThreadId.load());
        ui = node.ui;
    }
    Node &operator=(Node &&node)
    {
        if (this != &node)
        {
            ID = node.ID;
            Name = std::move(node.Name);
            Inputs = std::move(node.Inputs);
            Outputs = std::move(node.Outputs);
            Color = node.Color;
            Type = node.Type;
            Size = node.Size;
            Position = node.Position;
            State = std::move(node.State);
            SavedState = std::move(node.SavedState);
            OnExecute = node.OnExecute;
            LastExecuteResult = node.LastExecuteResult;
            BeginExecuteTime = node.BeginExecuteTime;
            EndExecuteTime = node.EndExecuteTime;
            ExecuteTime = node.ExecuteTime;
            IsRunning.store(node.IsRunning.load());
            RunningThreadId.store(node.RunningThreadId.load());
            ui = node.ui;
        }
        return *this;
    }

    bool can_execute()
    {
        for (auto input : Inputs)
            if (!input.can_execute())
                return false;
    }
    std::vector<Node *> get_last_nodes()
    {
        std::vector<Node *> nodes;
        for (auto input : Inputs)
        {
            if (input.Kind == PinKind::Input && input.Node)
            {
                nodes.push_back(input.Node);
            }
        }
        return nodes;
    }

    std::string get_last_execute_time()
    {
        if (ExecuteTime)
        {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(*ExecuteTime);
            return std::to_string(duration.count()) + " ms";
        }
        return "0 ms";
    }

    bool has_execute_mothod()
    {
        return OnExecute != nullptr;
    }

    auto execute(Graph *graph)
    {
        return OnExecuteEx(graph, this);
    }

    bool is_running()
    {
        return IsRunning.load();
    }

    size_t get_running_thread_id()
    {
        if (IsRunning.load())
        {
            return RunningThreadId.load();
        }
        return 0;
    }
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

    int next_id = 1;
    int get_next_id()
    {
        return next_id++;
    }

    void build_node(Node *node)
    {
        for (auto &input : node->Inputs)
        {
            input.Node = node;
            input.Kind = PinKind::Input;
        }

        for (auto &output : node->Outputs)
        {
            output.Node = node;
            output.Kind = PinKind::Output;
        }
    }

    void build_nodes()
    {
        for (auto &node : this->Nodes)
            build_node(&node);
    }

    struct ExectureEnv
    {

        std::atomic<bool> isRunning = false;
        std::atomic<bool> needRunning = false; // 在下一次循环中是否需要执行
        std::atomic<bool> isStoped = false;    // 是否已经销毁
        std::map<ed::NodeId, std::chrono::steady_clock::time_point> nodeBeginExecuteTime;
        std::map<ed::NodeId, std::chrono::steady_clock::time_point> nodeEndExecuteTime;
        std::map<ed::NodeId, std::chrono::steady_clock::duration> nodeExecuteTime;
        std::list<ed::NodeId> nodeBeginExecuteList;
        std::future<std::vector<ExecuteResult>> future; // 异步执行
        // need inint
        std::function<std::vector<ExecuteResult>(Graph *)> executeFunc;
        Graph *graph;
        Application *app;

        bool is_stoped()
        {
            return isStoped.load();
        }

        void need_stop()
        {
            isStoped.store(true);
        }

        void need_execute()
        {
            needRunning = true;
            async_execute();
        }

        void async_execute()
        {
            // 如果没有执行过，或者需要执行，且没有正在执行
            if (needRunning && !isRunning)
            {
                isRunning = true;
                future = std::async(std::launch::async, executeFunc, graph);
            }
            if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                auto results = future.get();
                for (auto result : results)
                {
                    if (result.has_error())
                    {
                        if (result.has_node_error())
                        {
                            auto node = graph->FindNode(std::get<ed::NodeId>(result.Error->Source));
                            if (node)
                            {
                                node->LastExecuteResult = result;
                            }
                        }
                        if (result.has_pin_error())
                        {
                            auto pin = graph->FindPin(std::get<ed::PinId>(result.Error->Source));
                            if (pin)
                            {
                                pin->Node->LastExecuteResult = result;
                            }
                        }
                        if (result.has_link_error())
                        {
                            auto link = graph->FindLink(std::get<ed::LinkId>(result.Error->Source));
                            if (link)
                            {
                                auto start_pin = graph->FindPin(link->StartPinID);
                                if (start_pin)
                                {
                                    start_pin->Node->LastExecuteResult = result;
                                }
                            }
                        }
                    }
                }
                isRunning = false;
                needRunning = false;
            }
        }
    };
    ExectureEnv env;

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

    bool serialize(std::string &json_buff);
    bool deserialize(const std::string &json_buff);
};

#include "node_serialize.hpp"

inline bool Graph::serialize(std::string &json_buff)
{
    json::object obj;
    json::array nodes;
    for (auto &node : Nodes)
    {
        nodes.push_back(json::serialize(&node, NodeSerializer()));
    }
    obj["nodes"] = nodes;
    json::array links;
    for (auto &link : Links)
    {
        links.push_back(json::serialize(&link, NodeSerializer()));
    }
    obj["links"] = links;
    json_buff = obj.dumps();
    return true;
}

inline bool Graph::deserialize(const std::string &json_buff)
{
    auto json_opt = json::parse(json_buff);
    if (!json_opt)
        return false;
    auto json = json_opt.value();
    {
        auto nodes = json.as_object().at("nodes").as_array();
        for (auto node : nodes)
        {
            Node n(0, "null");
            if (json::deserialize(node, n, NodeDeserializer()))
            {
                Graph g;
                static auto get_factory = [](const std::string &name) -> NodeWorldGlobal::NodeFactory_t
                {
                    for (auto &factories : NodeWorldGlobal::nodeFactories)
                    {
                        auto it = std::find_if(factories.second.begin(), factories.second.end(), [&name](const auto &factory)
                                               { return factory.first == name; });
                        if (it != factories.second.end())
                        {
                            return it->second;
                        }
                    }
                    return nullptr;
                };
                auto node_factory = get_factory(n.Name);
                if (node_factory)
                {
                    auto tmp_node = node_factory([&]()
                                                 { return g.get_next_id(); },
                                                 [&](Node *node)
                                                 { g.build_node(node); },
                                                 g.Nodes, this->env.app);
                    n.OnExecute = tmp_node->OnExecute;
                    for (auto &input : n.Inputs)
                    {
                        input.app = this->env.app;
                    }
                    for (auto &output : n.Outputs)
                    {
                        output.app = this->env.app;
                    }
                }
                Nodes.push_back(n);
                if (reinterpret_cast<int64>(n.ID.AsPointer()) > next_id)
                {
                    next_id = static_cast<int>(reinterpret_cast<int64>(n.ID.AsPointer()));
                }
                for (auto &input : n.Inputs)
                {
                    if (reinterpret_cast<int64>(input.ID.AsPointer()) > next_id)
                    {
                        next_id = static_cast<int>(reinterpret_cast<int64>(input.ID.AsPointer()));
                    }
                }
                for (auto &output : n.Outputs)
                {
                    if (reinterpret_cast<int64>(output.ID.AsPointer()) > next_id)
                    {
                        next_id = static_cast<int>(reinterpret_cast<int64>(output.ID.AsPointer()));
                    }
                }
            }
        }
        auto links = json.as_object().at("links").as_array();
        for (auto link : links)
        {
            Link l(0, 0, 0);
            if (json::deserialize(link, l, NodeDeserializer()))
            {
                Links.push_back(l);
            }

            if (reinterpret_cast<int64>(l.ID.AsPointer()) > next_id)
            {
                next_id = static_cast<int>(reinterpret_cast<int64>(l.ID.AsPointer()));
            }
        }
        return true;
    }
    return false;
}

struct NodeIdLess
{
    bool operator()(const ed::NodeId &lhs, const ed::NodeId &rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

#define try_catch_block                                        \
    node->BeginExecuteTime = std::chrono::steady_clock::now(); \
    try                                                        \
    {

#define catch_block_and_return                                           \
    }                                                                    \
    catch (const std::exception &e)                                      \
    {                                                                    \
        return ExecuteResult::ErrorNode(node->ID, e.what());             \
    }                                                                    \
    catch (...)                                                          \
    {                                                                    \
        return ExecuteResult::ErrorNode(node->ID, "Unknown error");      \
    }                                                                    \
    node->EndExecuteTime = std::chrono::steady_clock::now();             \
    node->ExecuteTime = *node->EndExecuteTime - *node->BeginExecuteTime; \
    return ExecuteResult::Success();

static ExecuteResult get_image(Graph *graph, Pin input, cv::Mat &image)
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
static ExecuteResult get_value(Graph *graph, Pin input, T &value)
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

namespace ui
{
    const static int PinIconSize = 24;

    static ImColor GetIconColor(PinType type)
    {
        switch (type)
        {
        default:
        case PinType::Flow:
            return ImColor(255, 255, 255);
        case PinType::Image:
            return ImColor(51, 150, 215);
        case PinType::Rect:
            return ImColor(220, 48, 48);
        case PinType::Size:
            return ImColor(220, 148, 48);
        case PinType::Point:
            return ImColor(220, 148, 148);
        case PinType::Bool:
            return ImColor(220, 48, 48);
        case PinType::Int:
            return ImColor(68, 201, 156);
        case PinType::Float:
            return ImColor(147, 226, 74);
        case PinType::String:
            return ImColor(124, 21, 153);
        case PinType::Object:
            return ImColor(51, 150, 215);
        case PinType::Function:
            return ImColor(218, 0, 183);
        case PinType::Delegate:
            return ImColor(255, 48, 48);
        }
    };

    static void DrawPinIcon(const Pin &pin, bool connected, int alpha)
    {
        IconType iconType;
        ImColor color = GetIconColor(pin.Type);
        color.Value.w = alpha / 255.0f;
        switch (pin.Type)
        {
        case PinType::Flow:
            iconType = IconType::Flow;
            break;
        case PinType::Bool:
            iconType = IconType::Circle;
            break;
        case PinType::Int:
            iconType = IconType::Circle;
            break;
        case PinType::Float:
            iconType = IconType::Circle;
            break;
        case PinType::String:
            iconType = IconType::Circle;
            break;
        case PinType::Image:
            iconType = IconType::RoundSquare;
            break;
        case PinType::Rect:
            iconType = IconType::Circle;
            break;
        case PinType::Size:
            iconType = IconType::Circle;
            break;
        case PinType::Point:
            iconType = IconType::Circle;
            break;
        case PinType::Color:
            iconType = IconType::Circle;
            break;

        case PinType::Contour:
            iconType = IconType::Circle;
            break;
        case PinType::KeyPoint:
            iconType = IconType::Circle;
            break;
        case PinType::Feature:
            iconType = IconType::Circle;
            break;
        case PinType::Match:
            iconType = IconType::Circle;
            break;

        case PinType::Contours:
            iconType = IconType::Grid;
            break;
        case PinType::KeyPoints:
            iconType = IconType::Grid;
            break;
        case PinType::Matches:
            iconType = IconType::Grid;
            break;
        case PinType::Circles:
            iconType = IconType::Grid;
            break;

        case PinType::Object:
            iconType = IconType::Circle;
            break;
        case PinType::Function:
            iconType = IconType::Circle;
            break;
        case PinType::Delegate:
            iconType = IconType::Square;
            break;
        default:
            return;
        }

        ax::Widgets::Icon(ImVec2(static_cast<float>(PinIconSize), static_cast<float>(PinIconSize)), iconType, connected, color, ImColor(32, 32, 32, alpha));
    };

    static ImColor rand_color_base_saturation(float base, float saturation)
    {
        return ImColor::HSV(rand() % 255 / 255.0f, saturation, base);
    }

    static ImColor get_color_from_thread_id(size_t thread_id)
    {
        static std::map<size_t, ImColor> colors;
        if (colors.find(thread_id) == colors.end())
        {
            colors[thread_id] = rand_color_base_saturation(0.6f, 0.6f);
        }
        return colors[thread_id];
    }
}

#endif // BASE_NODE_HPP