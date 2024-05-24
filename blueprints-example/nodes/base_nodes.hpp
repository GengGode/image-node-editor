#ifndef BASE_NODE_HPP
#define BASE_NODE_HPP

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
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

#include "../utilities/builders.h"
#include "../utilities/widgets.h"

#include "node_port_types.hpp"

static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect &rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

static inline void DrawItemRect(ImColor color, float expand = 0.0f)
{
    ImGui::GetWindowDrawList()->AddRect(
        ImGui::GetItemRectMin() - ImVec2(expand, expand),
        ImGui::GetItemRectMax() + ImVec2(expand, expand),
        color);
};

static inline void FillItemRect(ImColor color, float expand = 0.0f, float rounding = 0.0f)
{
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetItemRectMin() - ImVec2(expand, expand),
        ImGui::GetItemRectMax() + ImVec2(expand, expand),
        color, rounding);
};

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

enum class NodeType
{
    Blueprint,
    MaaTaskFlow,
    Win32,
    Win32Input,
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
    {"Maa任务流", NodeType::MaaTaskFlow},
    {"Win32", NodeType::Win32},
    {"Win32输入", NodeType::Win32Input},
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
    static std::map<std::pair<PinType, PinType>, NodeFactory_t> registerLinkAutoConvertNodeFactories;
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
    void event_value_changed();

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

static inline bool CanCreateLink(Pin *a, Pin *b)
{
    if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
        return false;
    return true;
}

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
    Graph *graph;
    // 节点是否折叠为小图标
    bool is_expanded = true;
    // 折叠时汇总的输入连线
    std::vector<Link> virtual_input_links;
    // 折叠时汇总的输出连线
    std::vector<Link> virtual_output_links;

    std::shared_ptr<Pin> virtual_input = nullptr;
    std::shared_ptr<Pin> virtual_output = nullptr;

    Pin &get_virtual_input();

    Pin &get_virtual_output();

    void draw_node_ui(Node *node);
    void draw_input_pin(Pin &input);
    void draw_output_pin(Pin &output);
    void draw_node_title(Node *node);
    void draw_node_body(Node *node);
    void draw_node_footer(Node *node);

    void draw_node(Node *node);
    void draw_comment_node(Node *node);
    void draw_simple_node(Node *node);
    void draw_image_node(Node *node);
    void draw_flow_node(Node *node);
};

struct node_state_value
{
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

    Graph *graph;

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
    std::shared_ptr<node_state_value> state_value;

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

    void execute(Graph *graph)
    {
        LastExecuteResult = OnExecuteEx(graph, this);
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

    void expand();
    void collapse();

    //
    bool is_simple_node()
    {
        return Type == NodeType::Simple;
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

struct GraphUi
{
    Graph *graph;
    util::BlueprintNodeBuilder *builder;
    std::optional<ErrorInfo> context_error_opt;
    Pin *new_link_pin;
    bool has_error;
    bool has_error_and_hovered_on_port;
    bool has_error_and_hovered_on_node;

    void draw_node_input_pins(Node *node);
    void draw_node_output_pins(Node *node);
    void draw_nodes();
    void draw_links();
    void draw_virtual_links();
};

struct Graph
{
    std::vector<Node> Nodes;
    std::vector<Link> Links;
    // 折叠节点时节点的连线汇总为虚拟连线
    // std::vector<std::pair<ed::PinId, ed::PinId>> virtualLinks;
    // std::vector<Link> virtualLinks;

    GraphUi ui;

    int next_id = 1;
    int get_next_id()
    {
        return next_id++;
    }

    void build_node(Node *node)
    {
        node->graph = this;
        node->ui.graph = this;
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
        std::optional<std::chrono::steady_clock::time_point> BeginExecuteTime;
        std::optional<std::chrono::steady_clock::time_point> EndExecuteTime;
        std::optional<std::chrono::steady_clock::duration> ExecuteTime;
        std::atomic<double> all_execute_time = 0;
        std::list<ed::NodeId> nodeBeginExecuteList;
        std::future<void> future; // 异步执行
        // need inint
        void execture_stopwatch()
        {
            BeginExecuteTime = std::chrono::steady_clock::now();
            ExecuteNodes();
            EndExecuteTime = std::chrono::steady_clock::now();
            ExecuteTime = std::chrono::duration_cast<std::chrono::milliseconds>(*EndExecuteTime - *BeginExecuteTime);
            all_execute_time = static_cast<double>(ExecuteTime->count());
        };
        Graph *graph;
        Application *app;

        void ExecuteNode(Node *node)
        {
            if (node->has_execute_mothod())
                node->execute(graph);
        }

        void ExecuteNodes()
        {
            // 每个节点有一个依赖表，记录依赖的节点
            std::map<Node *, std::vector<Node *>> depend_map;
            // 每个节点都有一个关联表，记录依赖它的节点
            std::map<Node *, std::set<Node *>> relate_map;
            // 每一个节点有一个是否成功运行完毕的标志
            std::map<Node *, bool> node_run_map;
            // 如果节点运行错误，记录错误节点和它的关联节点
            std::set<Node *> error_and_relate_nodes;
            // 需要运行的所有节点
            std::list<Node *> need_run_nodes;
            // 运行结束标志
            bool is_end = false;
            // 理论上运行的循环次数不会超过节点数
            size_t max_loop_limit = graph->Nodes.size();
            // 运行循环次数
            int loop_count = 0;

            // 需要运行所有节点
            for (auto &node : graph->Nodes)
                need_run_nodes.push_back(&node);

            // 生成依赖表
            {
                // 根据输入连接生成依赖表
                for (auto &node : graph->Nodes)
                {
                    for (auto &input : node.Inputs)
                    {
                        if (graph->IsPinLinked(input.ID) == false)
                            continue;
                        auto links = graph->FindPinLinks(input.ID);
                        for (auto &link : links)
                        {
                            auto beginpin = graph->FindPin(link->StartPinID);
                            if (beginpin == nullptr)
                                continue;
                            if (beginpin->Kind != PinKind::Output)
                                continue;
                            auto beginnode = beginpin->Node;
                            if (beginnode == nullptr)
                                continue;
                            depend_map[&node].push_back(beginnode);
                        }
                    }
                }
                // 根据输出连接生成关联表
                for (auto &node : graph->Nodes)
                {
                    for (auto &output : node.Outputs)
                    {
                        if (graph->IsPinLinked(output.ID) == false)
                            continue;
                        auto links = graph->FindPinLinks(output.ID);
                        for (auto &link : links)
                        {
                            auto endpin = graph->FindPin(link->EndPinID);
                            if (endpin == nullptr)
                                continue;
                            if (endpin->Kind != PinKind::Input)
                                continue;
                            auto endnode = endpin->Node;
                            if (endnode == nullptr)
                                continue;
                            relate_map[&node].insert(endnode);
                        }
                    }
                }
            }

            while (!is_end)
            {
                const auto get_depend_over_nodes = [&need_run_nodes, &depend_map, &node_run_map, error_and_relate_nodes]()
                {
                    std::vector<Node *> nodes;
                    for (auto &node : need_run_nodes)
                    {
                        // 跳过已经运行过的节点
                        if (node_run_map[node] == true)
                            continue;
                        // 跳过在[错误和关联节点]中的节点
                        if (error_and_relate_nodes.find(node) != error_and_relate_nodes.end())
                            continue;

                        auto &depend_nodes = depend_map[node];
                        // 如果没有依赖或者依赖节点都已经运行完毕
                        if (depend_nodes.size() == 0)
                        {
                            nodes.push_back(node);
                            continue;
                        }
                        bool is_all_depend_over = true;
                        for (auto &depend : depend_nodes)
                        {
                            if (node_run_map[depend] == false)
                            {
                                is_all_depend_over = false;
                                break;
                            }
                        }
                        if (is_all_depend_over)
                        {
                            nodes.push_back(node);
                        }
                    }

                    return nodes;
                };
                // 获取依赖节点都已经运行完毕的节点
                auto can_run_nodes = get_depend_over_nodes();
                // 没有可以运行的节点，终止循环
                if (can_run_nodes.size() == 0)
                {
                    is_end = true;
                    break;
                }
                // 并行执行节点
                std::vector<std::future<void>> futures;
                for (auto &node : can_run_nodes)
                { // 添加到运行节点列表
                    futures.push_back(std::async(std::launch::async, [this, node]
                                                 { ExecuteNode(node); }));
                }

                // debug 打印本次循环执行的节点
                printf("本%d次循环执行的节点: %zd个", loop_count, can_run_nodes.size());
                // ImGui::InsertNotification({ImGuiToastType::Info, 3000, (std::string("本次运行第") + std::to_string(loop_count) + "次循环执行的节点: " + std::to_string(can_run_nodes.size()) + "个").c_str()});
                for (auto &node : can_run_nodes)
                {
                    printf("%d ", static_cast<int>(reinterpret_cast<int64>(node->ID.AsPointer())));
                }
                printf("\n");

                // 等待所有节点运行完毕
                for (auto &future : futures)
                    future.get();

                // 标记节点已经运行完毕
                for (auto &node : can_run_nodes)
                    node_run_map[node] = true;

                // 如果节点运行错误，记录错误节点
                std::set<Node *> error_nodes;
                for (auto &node : can_run_nodes)
                {
                    if (node->LastExecuteResult.has_error())
                        error_nodes.insert(node);
                }

                // 从【需要运行的节点】中删除已经运行完成的
                for (auto &node : can_run_nodes)
                {
                    need_run_nodes.remove(node);
                }

                // 将[错误节点]和[关联节点]加入到[错误和关联节点]中
                for (auto &error_node : error_nodes)
                {
                    error_and_relate_nodes.insert(error_node);
                    for (auto &relate_node : relate_map[error_node])
                    {
                        error_and_relate_nodes.insert(relate_node);
                    }
                }

                // 循环次数超过限制，终止循环
                if (loop_count++ > max_loop_limit)
                {
                    is_end = true;
                    break;
                }
            }
        }

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
                const auto execute_and_release = [this](Graph *graph)
                {
                    execture_stopwatch();
                    isRunning = false;
                    needRunning = false;
                };
                isRunning = true;
                future = std::async(std::launch::async, execute_and_release, graph);
            }
            if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                future.get();
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

    void auto_arrange();

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
                    n.state_value = tmp_node->state_value;
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