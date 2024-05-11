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

#include <json.hpp>

struct Serialize
{
    json::value operator()(const ImVec2 &v) const
    {
        return json::object{{"ImVec2", json::array{v.x, v.y}}};
    }
    json::value operator()(const ImColor &v) const
    {
        return json::object{{"ImColor", json::array{v.Value.x, v.Value.y, v.Value.z, v.Value.w}}};
    }
    json::value operator()(const cv::Rect &v) const
    {
        return json::object{{"cv::Rect", json::array{v.x, v.y, v.width, v.height}}};
    }
    json::value operator()(const cv::Size &v) const
    {
        return json::object{{"cv::Size", json::array{v.width, v.height}}};
    }
    json::value operator()(const cv::Point &v) const
    {
        return json::object{{"cv::Point", json::array{v.x, v.y}}};
    }
    json::value operator()(const cv::Scalar &v) const
    {
        return json::object{{"cv::Scalar", json::array{v[0], v[1], v[2], v[3]}}};
    }
    json::value operator()(const Contour &v) const
    {
        json::array arr;
        for (auto point : v)
        {
            arr.push_back(json::array{point.x, point.y});
        }
        return json::object{{"Contour", arr}};
    }
    json::value operator()(const Contours &v) const
    {
        json::array arr;
        for (auto contour : v)
        {
            json::array contour_arr;
            for (auto point : contour)
            {
                contour_arr.push_back(json::array{point.x, point.y});
            }
            arr.push_back(contour_arr);
        }
        return json::object{{"Contours", arr}};
    }
    json::value operator()(const cv::KeyPoint &v) const
    {
        return json::object{{"cv::KeyPoint", json::array{v.pt.x, v.pt.y, v.size, v.angle, v.response, v.octave, v.class_id}}};
    }
    json::value operator()(const KeyPoints &v) const
    {
        json::array arr;
        for (auto keypoint : v)
        {
            arr.push_back(json::array{keypoint.pt.x, keypoint.pt.y, keypoint.size, keypoint.angle, keypoint.response, keypoint.octave, keypoint.class_id});
        }
        return json::object{{"KeyPoints", arr}};
    }
    json::value operator()(const Feature &v) const
    {
        json::array keypoint_arr;
        for (auto keypoint : v.first)
        {
            keypoint_arr.push_back(json::array{keypoint.pt.x, keypoint.pt.y, keypoint.size, keypoint.angle, keypoint.response, keypoint.octave, keypoint.class_id});
        }
        return json::object{{"Feature", json::array{keypoint_arr, "cv::Mat"}}};
    }
    json::value operator()(const cv::DMatch &v) const
    {
        return json::object{{"cv::DMatch", json::array{v.queryIdx, v.trainIdx, v.imgIdx, v.distance}}};
    }
    json::value operator()(const Matches &v) const
    {
        json::array arr;
        for (auto match : v)
        {
            arr.push_back(json::array{match.queryIdx, match.trainIdx, match.imgIdx, match.distance});
        }
        return json::object{{"Matches", arr}};
    }
    json::value operator()(const Circles &v) const
    {
        json::array arr;
        for (auto circle : v)
        {
            arr.push_back(json::array{circle[0], circle[1], circle[2]});
        }
        return json::object{{"Circles", arr}};
    }

    json::value operator()(const port_value_t &v) const
    {
        if (std::holds_alternative<int>(v))
        {
            return json::object{{"int", std::get<int>(v)}};
        }
        if (std::holds_alternative<float>(v))
        {
            return json::object{{"float", std::get<float>(v)}};
        }
        if (std::holds_alternative<bool>(v))
        {
            return json::object{{"bool", std::get<bool>(v)}};
        }
        if (std::holds_alternative<std::string>(v))
        {
            return json::object{{"string", std::get<std::string>(v)}};
        }
        if (std::holds_alternative<cv::Mat>(v))
        {
            return json::object{{"cv::Mat", "cv::Mat"}};
        }
        if (std::holds_alternative<cv::Rect>(v))
        {
            return json::object{{"cv::Rect", json::serialize(std::get<cv::Rect>(v), *this)}};
        }
        if (std::holds_alternative<cv::Size>(v))
        {
            return json::object{{"cv::Size", json::serialize(std::get<cv::Size>(v), *this)}};
        }
        if (std::holds_alternative<cv::Point>(v))
        {
            return json::object{{"cv::Point", json::serialize(std::get<cv::Point>(v), *this)}};
        }
        if (std::holds_alternative<cv::Scalar>(v))
        {
            return json::object{{"cv::Scalar", json::serialize(std::get<cv::Scalar>(v), *this)}};
        }
        if (std::holds_alternative<Contour>(v))
        {
            return json::object{{"Contour", json::serialize(std::get<Contour>(v), *this)}};
        }
        if (std::holds_alternative<Contours>(v))
        {
            return json::object{{"Contours", json::serialize(std::get<Contours>(v), *this)}};
        }
        if (std::holds_alternative<cv::KeyPoint>(v))
        {
            return json::object{{"cv::KeyPoint", json::serialize(std::get<cv::KeyPoint>(v), *this)}};
        }
        if (std::holds_alternative<KeyPoints>(v))
        {
            return json::object{{"KeyPoints", json::serialize(std::get<KeyPoints>(v), *this)}};
        }
        if (std::holds_alternative<Feature>(v))
        {
            return json::object{{"Feature", json::serialize(std::get<Feature>(v), *this)}};
        }
        if (std::holds_alternative<cv::DMatch>(v))
        {
            return json::object{{"cv::DMatch", json::serialize(std::get<cv::DMatch>(v), *this)}};
        }
        if (std::holds_alternative<Matches>(v))
        {
            return json::object{{"Matches", json::serialize(std::get<Matches>(v), *this)}};
        }
        if (std::holds_alternative<Circles>(v))
        {
            return json::object{{"Circles", json::serialize(std::get<Circles>(v), *this)}};
        }
        return json::object{{"null", "null"}};
    }

    json::value operator()(const Node *node) const
    {
        json::object obj;
        obj["type"] = "node";
        obj["node_id"] = reinterpret_cast<int64>(node->ID.AsPointer());
        obj["node_name"] = node->Name;
        obj["node_type"] = static_cast<int>(node->Type);
        obj["node_type_name"] = nodeTypes.at(static_cast<int>(node->Type)).first;
        obj["node_color"] = json::serialize(node->Color, *this);
        obj["node_size"] = json::serialize(node->Size, *this);
        obj["node_position"] = json::serialize(node->Position, *this);
        json::array inputs;
        for (auto input : node->Inputs)
        {
            json::object input_obj;
            input_obj["type"] = "input";
            input_obj["input_id"] = reinterpret_cast<int64>(input.ID.AsPointer());
            input_obj["input_name"] = input.Name;
            input_obj["input_type"] = static_cast<int>(input.Type);
            input_obj["input_type_label"] = typeLabelNames.at(input.Type);
            input_obj["input_value"] = json::serialize(input.Value, *this);
            input_obj["input_kind"] = static_cast<int>(input.Kind);
            inputs.push_back(input_obj);
        }
        obj["inputs"] = inputs;
        json::array outputs;
        for (auto output : node->Outputs)
        {
            json::object output_obj;
            output_obj["type"] = "output";
            output_obj["output_id"] = reinterpret_cast<int64>(output.ID.AsPointer());
            output_obj["output_name"] = output.Name;
            output_obj["output_type"] = static_cast<int>(output.Type);
            output_obj["output_type_label"] = typeLabelNames.at(output.Type);
            output_obj["output_value"] = json::serialize(output.Value, *this);
            output_obj["output_kind"] = static_cast<int>(output.Kind);
            outputs.push_back(output_obj);
        }
        obj["outputs"] = outputs;
        return obj;
    }

    json::value operator()(const Link *link) const
    {
        json::object obj;
        obj["type"] = "link";
        obj["link_id"] = reinterpret_cast<int64>(link->ID.AsPointer());
        obj["link_start_pin_id"] = reinterpret_cast<int64>(link->StartPinID.AsPointer());
        obj["link_end_pin_id"] = reinterpret_cast<int64>(link->EndPinID.AsPointer());
        obj["link_color"] = json::serialize(link->Color, *this);
        return obj;
    }
};

struct Deserializer
{
    bool operator()(const json::value &json, ImVec2 &v) const
    {
        if (json.is_object() && json.as_object().contains("ImVec2"))
        {
            auto arr = json.as_object().at("ImVec2").as_array();
            v.x = arr[0].as_float();
            v.y = arr[1].as_float();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, ImColor &v) const
    {
        if (json.is_object() && json.as_object().contains("ImColor"))
        {
            auto arr = json.as_object().at("ImColor").as_array();
            v.Value.x = arr[0].as_float();
            v.Value.y = arr[1].as_float();
            v.Value.z = arr[2].as_float();
            v.Value.w = arr[3].as_float();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::Rect &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::Rect"))
        {
            auto arr = json.as_object().at("cv::Rect").as_array();
            v.x = arr[0].as_integer();
            v.y = arr[1].as_integer();
            v.width = arr[2].as_integer();
            v.height = arr[3].as_integer();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::Size &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::Size"))
        {
            auto arr = json.as_object().at("cv::Size").as_array();
            v.width = arr[0].as_integer();
            v.height = arr[1].as_integer();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::Point &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::Point"))
        {
            auto arr = json.as_object().at("cv::Point").as_array();
            v.x = arr[0].as_integer();
            v.y = arr[1].as_integer();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::Scalar &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::Scalar"))
        {
            auto arr = json.as_object().at("cv::Scalar").as_array();
            v[0] = arr[0].as_float();
            v[1] = arr[1].as_float();
            v[2] = arr[2].as_float();
            v[3] = arr[3].as_float();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Contour &v) const
    {
        if (json.is_object() && json.as_object().contains("Contour"))
        {
            auto arr = json.as_object().at("Contour").as_array();
            for (auto point_json : arr)
            {
                cv::Point point;
                if (operator()(point_json, point))
                {
                    v.push_back(point);
                }
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Contours &v) const
    {
        if (json.is_object() && json.as_object().contains("Contours"))
        {
            auto arr = json.as_object().at("Contours").as_array();
            for (auto contour : arr)
            {
                Contour c;
                if (operator()(contour, c))
                {
                    v.push_back(c);
                }
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::KeyPoint &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::KeyPoint"))
        {
            auto arr = json.as_object().at("cv::KeyPoint").as_array();
            v.pt.x = arr[0].as_float();
            v.pt.y = arr[1].as_float();
            v.size = arr[2].as_float();
            v.angle = arr[3].as_float();
            v.response = arr[4].as_float();
            v.octave = arr[5].as_integer();
            v.class_id = arr[6].as_integer();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, KeyPoints &v) const
    {
        if (json.is_object() && json.as_object().contains("KeyPoints"))
        {
            auto arr = json.as_object().at("KeyPoints").as_array();
            for (auto keypoint : arr)
            {
                cv::KeyPoint kp;
                if (operator()(keypoint, kp))
                {
                    v.push_back(kp);
                }
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Feature &v) const
    {
        if (json.is_object() && json.as_object().contains("Feature"))
        {
            auto arr = json.as_object().at("Feature").as_array();
            KeyPoints keypoint;
            if (operator()(arr[0], keypoint))
            {
                v.first = keypoint;
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::DMatch &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::DMatch"))
        {
            auto arr = json.as_object().at("cv::DMatch").as_array();
            v.queryIdx = arr[0].as_integer();
            v.trainIdx = arr[1].as_integer();
            v.imgIdx = arr[2].as_integer();
            v.distance = arr[3].as_float();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Matches &v) const
    {
        if (json.is_object() && json.as_object().contains("Matches"))
        {
            auto arr = json.as_object().at("Matches").as_array();
            for (auto match : arr)
            {
                cv::DMatch m;
                if (operator()(match, m))
                {
                    v.push_back(m);
                }
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Circles &v) const
    {
        if (json.is_object() && json.as_object().contains("Circles"))
        {
            auto arr = json.as_object().at("Circles").as_array();
            for (auto circle : arr)
            {
                v.push_back({circle[0].as_float(), circle[1].as_float(), circle[2].as_float()});
            }
            return true;
        }
        return false;
    }

    bool operator()(const json::value &json, port_value_t &v) const
    {
        if (json.is_object())
        {
            if (json.as_object().contains("int"))
            {
                v = json.as_object().at("int").as_integer();
                return true;
            }
            if (json.as_object().contains("float"))
            {
                v = json.as_object().at("float").as_float();
                return true;
            }
            if (json.as_object().contains("bool"))
            {
                v = json.as_object().at("bool").as_boolean();
                return true;
            }
            if (json.as_object().contains("string"))
            {
                v = json.as_object().at("string").as_string();
                return true;
            }
            if (json.as_object().contains("cv::Mat"))
            {
                v = cv::Mat();
                return true;
            }
            if (json.as_object().contains("cv::Rect"))
            {
                cv::Rect rect;
                if (operator()(json.as_object().at("cv::Rect"), rect))
                {
                    v = rect;
                    return true;
                }
            }
            if (json.as_object().contains("cv::Size"))
            {
                cv::Size size;
                if (operator()(json.as_object().at("cv::Size"), size))
                {
                    v = size;
                    return true;
                }
            }
            if (json.as_object().contains("cv::Point"))
            {
                cv::Point point;
                if (operator()(json.as_object().at("cv::Point"), point))
                {
                    v = point;
                    return true;
                }
            }
            if (json.as_object().contains("cv::Scalar"))
            {
                cv::Scalar scalar;
                if (operator()(json.as_object().at("cv::Scalar"), scalar))
                {
                    v = scalar;
                    return true;
                }
            }
            if (json.as_object().contains("Contour"))
            {
                Contour contour;
                if (operator()(json.as_object().at("Contour"), contour))
                {
                    v = contour;
                    return true;
                }
            }
            if (json.as_object().contains("Contours"))
            {
                Contours contours;
                if (operator()(json.as_object().at("Contours"), contours))
                {
                    v = contours;
                    return true;
                }
            }
            if (json.as_object().contains("cv::KeyPoint"))
            {
                cv::KeyPoint keypoint;
                if (operator()(json.as_object().at("cv::KeyPoint"), keypoint))
                {
                    v = keypoint;
                    return true;
                }
            }
            if (json.as_object().contains("KeyPoints"))
            {
                KeyPoints keypoints;
                if (operator()(json.as_object().at("KeyPoints"), keypoints))
                {
                    v = keypoints;
                    return true;
                }
            }
            if (json.as_object().contains("Feature"))
            {
                Feature feature;
                if (operator()(json.as_object().at("Feature"), feature))
                {
                    v = feature;
                    return true;
                }
            }
            if (json.as_object().contains("cv::DMatch"))
            {
                cv::DMatch match;
                if (operator()(json.as_object().at("cv::DMatch"), match))
                {
                    v = match;
                    return true;
                }
            }
            if (json.as_object().contains("Matches"))
            {
                Matches matches;
                if (operator()(json.as_object().at("Matches"), matches))
                {
                    v = matches;
                    return true;
                }
            }
            if (json.as_object().contains("Circles"))
            {
                Circles circles;
                if (operator()(json.as_object().at("Circles"), circles))
                {
                    v = circles;
                    return true;
                }
            }
        }
        return false;
    }

    bool operator()(const json::value &json, Node &node) const
    {
        if (json.is_object() && json.as_object().contains("type") && json.as_object().at("type").as_string() == "node")
        {
            int id = json.as_object().at("node_id").as_integer();
            node.ID = ed::NodeId(id);
            node.Name = json.as_object().at("node_name").as_string();
            node.Type = NodeType(json.as_object().at("node_type").as_integer());
            json::deserialize(json.as_object().at("node_color"), node.Color, *this);
            json::deserialize(json.as_object().at("node_size"), node.Size, *this);
            json::deserialize(json.as_object().at("node_position"), node.Position, *this);
            auto inputs = json.as_object().at("inputs").as_array();
            for (auto input : inputs)
            {
                int input_id = input.as_object().at("input_id").as_integer();
                std::string input_name = input.as_object().at("input_name").as_string();
                PinType input_type = PinType(input.as_object().at("input_type").as_integer());
                port_value_t input_value;
                json::deserialize(input.as_object().at("input_value"), input_value, *this);
                PinKind input_kind = PinKind(input.as_object().at("input_kind").as_integer());
                Pin pin(input_id, input_name.c_str(), input_type, input_value);
                pin.Kind = PinKind(input.as_object().at("input_kind").as_integer());
                pin.Node = &node;
                node.Inputs.push_back(pin);
            }
            auto outputs = json.as_object().at("outputs").as_array();
            for (auto output : outputs)
            {
                int output_id = output.as_object().at("output_id").as_integer();
                std::string output_name = output.as_object().at("output_name").as_string();
                PinType output_type = PinType(output.as_object().at("output_type").as_integer());
                port_value_t output_value;
                json::deserialize(output.as_object().at("output_value"), output_value, *this);
                PinKind output_kind = PinKind(output.as_object().at("output_kind").as_integer());
                Pin pin(output_id, output_name.c_str(), output_type, output_value);
                pin.Kind = PinKind(output.as_object().at("output_kind").as_integer());
                pin.Node = &node;
                node.Outputs.push_back(pin);
            }
            return true;
        }
        return false;
    }

    bool operator()(const json::value &json, Link &link) const
    {
        if (json.is_object() && json.as_object().contains("type") && json.as_object().at("type").as_string() == "link")
        {
            int id = json.as_object().at("link_id").as_integer();
            link.ID = ed::LinkId(id);
            link.StartPinID = ed::PinId(json.as_object().at("link_start_pin_id").as_integer());
            link.EndPinID = ed::PinId(json.as_object().at("link_end_pin_id").as_integer());
            json::deserialize(json.as_object().at("link_color"), link.Color, *this);
            return true;
        }
        return false;
    }
};

inline bool Graph::serialize(std::string &json_buff)
{
    json::object obj;
    json::array nodes;
    for (auto &node : Nodes)
    {
        nodes.push_back(json::serialize(&node, Serialize()));
    }
    obj["nodes"] = nodes;
    json::array links;
    for (auto &link : Links)
    {
        links.push_back(json::serialize(&link, Serialize()));
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
            if (json::deserialize(node, n, Deserializer()))
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
            if (json::deserialize(link, l, Deserializer()))
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