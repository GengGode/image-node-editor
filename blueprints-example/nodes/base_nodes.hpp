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
#include <atomic>
#include <future>

#include <opencv2/opencv.hpp>

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

typedef std::vector<cv::Point> Contour;
typedef std::vector<Contour> Contours;
typedef std::vector<cv::KeyPoint> KeyPoints;
typedef std::pair<KeyPoints, cv::Mat> Feature;
typedef std::vector<cv::DMatch> Matches;
typedef std::vector<cv::Vec3f> Circles;

typedef std::variant<int, float, bool, std::string,
                     cv::Mat, cv::Rect, cv::Size, cv::Point, cv::Scalar,
                     Contour, Contours,
                     cv::KeyPoint, KeyPoints, Feature, cv::DMatch, Matches,
                     Circles>
    port_value_t;

struct MainThread
{
    inline static std::thread::id id = std::this_thread::get_id();
};

enum class PinType
{
    Flow,
    Image,
    Rect,
    Size,
    Point,
    Color,
    Contour,
    Contours,
    KeyPoint,
    KeyPoints,
    Feature,
    Match,
    Matches,
    Circles,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
    Delegate,
};

const std::map<std::size_t, PinType> typeMap = {
    {typeid(int).hash_code(), PinType::Int},
    {typeid(float).hash_code(), PinType::Float},
    {typeid(bool).hash_code(), PinType::Bool},
    {typeid(std::string).hash_code(), PinType::String},
    {typeid(cv::Mat).hash_code(), PinType::Image},
    {typeid(cv::Rect).hash_code(), PinType::Rect},
    {typeid(cv::Size).hash_code(), PinType::Size},
    {typeid(cv::Point).hash_code(), PinType::Point},
    {typeid(cv::Scalar).hash_code(), PinType::Color},
    {typeid(Contour).hash_code(), PinType::Contour},
    {typeid(Contours).hash_code(), PinType::Contours},
    {typeid(cv::KeyPoint).hash_code(), PinType::KeyPoint},
    {typeid(KeyPoints).hash_code(), PinType::KeyPoints},
    {typeid(Feature).hash_code(), PinType::Feature},
    {typeid(cv::DMatch).hash_code(), PinType::Match},
    {typeid(Matches).hash_code(), PinType::Matches},
    {typeid(Circles).hash_code(), PinType::Circles},

};
template <typename T>
static bool is_equal(const T &lft, const T &rht)
{
    return lft == rht;
}

template <>
static bool is_equal(const cv::Mat &lft, const cv::Mat &rht)
{
    if (lft.empty() && rht.empty())
        return true;
    if (lft.empty() || rht.empty())
        return false;

    if (lft.dims == rht.dims &&
        lft.size == rht.size &&
        lft.elemSize() == rht.elemSize())
    {
        if (lft.isContinuous() && rht.isContinuous())
        {
            return 0 == memcmp(lft.ptr(), rht.ptr(), lft.total() * lft.elemSize());
        }
        else
        {
            const cv::Mat *arrays[] = {&lft, &rht, 0};
            uchar *ptrs[2];
            cv::NAryMatIterator it(arrays, ptrs, 2);
            for (unsigned int p = 0; p < it.nplanes; p++, ++it)
                if (0 != memcmp(it.ptrs[0], it.ptrs[1], it.size * lft.elemSize()))
                    return false;

            return true;
        }
    }

    return false;
}

template <>
static bool is_equal(const Contour &lft, const Contour &rht)
{
    if (lft.size() != rht.size())
        return false;
    for (size_t i = 0; i < lft.size(); i++)
    {
        if (lft[i] != rht[i])
            return false;
    }
    return true;
}

template <>
static bool is_equal(const Contours &lft, const Contours &rht)
{
    if (lft.size() != rht.size())
        return false;
    for (size_t i = 0; i < lft.size(); i++)
    {
        if (!is_equal(lft[i], rht[i]))
            return false;
    }
    return true;
}

template <>
static bool is_equal(const cv::KeyPoint &lft, const cv::KeyPoint &rht)
{
    return lft.pt == rht.pt &&
           lft.size == rht.size &&
           lft.angle == rht.angle &&
           lft.response == rht.response &&
           lft.octave == rht.octave &&
           lft.class_id == rht.class_id;
}

template <>
static bool is_equal(const KeyPoints &lft, const KeyPoints &rht)
{
    if (lft.size() != rht.size())
        return false;
    for (size_t i = 0; i < lft.size(); i++)
    {
        if (!is_equal(lft[i], rht[i]))
            return false;
    }
    return true;
}

template <>
static bool is_equal(const Feature &lft, const Feature &rht)
{
    return is_equal(lft.first, rht.first) &&
           is_equal(lft.second, rht.second);
}

template <>
static bool is_equal(const cv::DMatch &lft, const cv::DMatch &rht)
{
    return lft.queryIdx == rht.queryIdx &&
           lft.trainIdx == rht.trainIdx &&
           lft.imgIdx == rht.imgIdx &&
           lft.distance == rht.distance;
}

template <>
static bool is_equal(const Matches &lft, const Matches &rht)
{
    if (lft.size() != rht.size())
        return false;
    for (size_t i = 0; i < lft.size(); i++)
    {
        if (!is_equal(lft[i], rht[i]))
            return false;
    }
    return true;
}

static bool is_equal(const port_value_t &lft, const port_value_t &rht)
{
    return std::visit([&](auto &&arg1, auto &&arg2)
                      { if (typeid(arg1)!= typeid(arg2))
                                return false;
                            return is_equal(arg1, arg2); },
                      lft, rht);
}

enum class PinKind
{
    Output,
    Input
};

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
        if (std::this_thread::get_id() != MainThread::id)
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

    ExecuteResult LastExecuteResult;
    std::optional<std::chrono::steady_clock::time_point> BeginExecuteTime;
    std::optional<std::chrono::steady_clock::time_point> EndExecuteTime;
    std::optional<std::chrono::steady_clock::duration> ExecuteTime;

    Node(int id, const char *name, ImColor color = ImColor(255, 255, 255)) : ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
    {
    }
    std::function<ExecuteResult(Graph *, Node *)> OnExecute = [](Graph *, Node *)
    {
        return ExecuteResult::ErrorCustom("Null Impl");
    };

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
    struct ExectureEnv
    {
        std::atomic<bool> isRunning = false;
        std::atomic<bool> needRunning = false; // 在下一次循环中是否需要执行
        std::map<ed::NodeId, std::chrono::steady_clock::time_point> nodeBeginExecuteTime;
        std::map<ed::NodeId, std::chrono::steady_clock::time_point> nodeEndExecuteTime;
        std::map<ed::NodeId, std::chrono::steady_clock::duration> nodeExecuteTime;
        std::list<ed::NodeId> nodeBeginExecuteList;
        std::future<std::vector<ExecuteResult>> future; // 异步执行
        // need inint
        std::function<std::vector<ExecuteResult>(Graph *)> executeFunc;
        Graph *graph;

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
    using FactoryGroupFunc_t = std::vector<std::pair<std::string, std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)>>>;
    static std::map<NodeType, FactoryGroupFunc_t> nodeFactories;
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
#endif // BASE_NODE_HPP