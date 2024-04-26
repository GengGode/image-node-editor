#define IMGUI_DEFINE_MATH_OPERATORS
#include <application.h>
#include "utilities/builders.h"
#include "utilities/widgets.h"
#include "notifiers/Notifier.hpp"

#include <imgui_node_editor.h>
#include <imgui_internal.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <functional>
#include <memory>

#include "nodes/base_node.hpp"

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
struct NodeIdLess
{
    bool operator()(const ed::NodeId &lhs, const ed::NodeId &rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

static ed::EditorContext *m_Editor = nullptr;

static bool Splitter(bool split_vertically, float thickness, float *size1, float *size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
    using namespace ImGui;
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

struct Example : public Application
{
    using Application::Application;

    int GetNextId()
    {
        return m_NextId++;
    }

    ed::LinkId GetNextLinkId()
    {
        return ed::LinkId(GetNextId());
    }

    bool CanCreateLink(std::shared_ptr<port> a, std::shared_ptr<port> b)
    {
        if (!a || !b || a == b || a->type != b->type || a->control_type == b->control_type)
            return false;
        return true;
    }

    void DrawItemRect(ImColor color, float expand = 0.0f)
    {
        ImGui::GetWindowDrawList()->AddRect(
            ImGui::GetItemRectMin() - ImVec2(expand, expand),
            ImGui::GetItemRectMax() + ImVec2(expand, expand),
            color);
    };

    void FillItemRect(ImColor color, float expand = 0.0f, float rounding = 0.0f)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetItemRectMin() - ImVec2(expand, expand),
            ImGui::GetItemRectMax() + ImVec2(expand, expand),
            color, rounding);
    };

    void ExecuteNode(ed::NodeId id)
    {
        auto node = env->find_node(id);
        if (!node)
            return;

        if (node->event.on_execute)
            node->event.on_execute(env->get_base_ptr());
    }
    bool ExecuteNode(std::shared_ptr<base_node> node)
    {
        if (node->event.on_execute)
            return node->event.on_execute(env->get_base_ptr());
        return false;
    }

    void ExecuteNodes()
    {
        Notifier::Add(Notif(Notif::Type::INFO, "ExecuteNodes"));
        std::list<std::shared_ptr<base_node>> begin_nodes;
        for (auto &node : env->nodes)
        {
            if (node->in_ports.size() == 0)
                begin_nodes.push_back(node);
        }
        printf("起点节点 .size() = %zd\n", begin_nodes.size());

        for (auto &node : begin_nodes)
        {
            auto current_node = node;
            while (current_node)
            {
                printf("当前运行节点->Name = %s\n", current_node->name.c_str());
                { // 没有OnExecute函数，终止执行链
                    if (!current_node->event.on_execute)
                    {
                        printf("node->OnExecute is null\n");
                        break;
                    }
                    // 执行当前节点
                    auto node_res = current_node->event.on_execute(env->get_base_ptr());
                    // 执行失败，终止执行链
                    if (!node_res)
                        break;
                    // 执行成功，继续执行链
                    // 没有输出，即最终节点，终止执行链
                    if (current_node->out_ports.size() == 0)
                        break;
                }
                // 有多个输出，打印输出数量
                printf("current_node->Outputs.size() = %zd\n", current_node->out_ports.size());
                static const auto output_get_next_node_func = [&](std::shared_ptr<out_port> output) -> std::shared_ptr<base_node>
                {
                    if (output->which_links.size() == 0)
                        return nullptr;
                    auto weak_link = output->which_links.back().lock();
                    if (weak_link)
                        return nullptr;
                    auto weak_pin = weak_link->which_to_in_port.lock();
                    if (weak_pin)
                        return nullptr;
                    return weak_pin->as_node_ptr.lock();
                };

                // 有一个输出，继续执行链 有多个输出，选择第一个输出，其他输出插入到队列末尾
                if (current_node->out_ports.size() >= 1)
                {
                    auto output = current_node->out_ports[0];
                    // 输出有连接，继续执行链
                    if (env->is_pin_linked(output->uuid) == false)
                    {
                        // 输出没有连接，终止执行链
                        break;
                    }

                    for (int i = 1; i < current_node->out_ports.size(); i++)
                    {
                        auto next_node = output_get_next_node_func(current_node->out_ports[i]);
                        if (next_node)
                            begin_nodes.push_back(next_node);
                    }

                    current_node = output_get_next_node_func(output);
                }
            }
            printf("执行的最后一个节点->Name = %s\n", current_node->name.c_str());
        }
    }

    void OnStart() override
    {
        ed::Config config;

        config.SettingsFile = "Blueprints.json";

        config.UserPointer = this;

        config.LoadNodeSettings = [](ed::NodeId nodeId, char *data, void *userPointer) -> size_t
        {
            auto self = static_cast<Example *>(userPointer);

            auto node = self->env->find_node(nodeId);
            if (!node)
                return 0;

            // if (data != nullptr)
            //     memcpy(data, node->State.data(), node->State.size());
            // return node->State.size();
            return 0;
        };

        config.SaveNodeSettings = [](ed::NodeId nodeId, const char *data, size_t size, ed::SaveReasonFlags reason, void *userPointer) -> bool
        {
            auto self = static_cast<Example *>(userPointer);

            auto node = self->env->find_node(nodeId);
            if (!node)
                return false;

            // node->State.assign(data, size);

            // self->TouchNode(nodeId);

            return true;
        };

        m_Editor = ed::CreateEditor(&config);
        ed::SetCurrentEditor(m_Editor);
        ed::NavigateToContent();

        m_HeaderBackground = LoadTexture("data/BlueprintBackground.png");
        // m_SaveIcon = LoadTexture("data/ic_save_white_24dp.png");
        // m_RestoreIcon = LoadTexture("data/ic_restore_white_24dp.png");
    }

    void OnStop() override
    {
        auto releaseTexture = [this](ImTextureID &id)
        {
            if (id)
            {
                DestroyTexture(id);
                id = nullptr;
            }
        };

        // releaseTexture(m_RestoreIcon);
        // releaseTexture(m_SaveIcon);
        // releaseTexture(m_HeaderBackground);

        if (m_Editor)
        {
            ed::DestroyEditor(m_Editor);
            m_Editor = nullptr;
        }
    }

    ImColor GetIconColor(global_env::value_type type)
    {
        switch (type)
        {
        default:
        case global_env::value_type::inout_int:
            return ImColor(68, 201, 156);
        case global_env::value_type::inout_float:
            return ImColor(147, 226, 74);
        case global_env::value_type::inout_bool:
            return ImColor(220, 48, 48);
        case global_env::value_type::inout_string:
            return ImColor(124, 21, 153);
        case global_env::value_type::inout_image:
            return ImColor(51, 150, 215);

            // case PinType::Flow:
            //     return ImColor(255, 255, 255);
            // case PinType::Bool:
            //     return ImColor(220, 48, 48);
            // case PinType::Int:
            //     return ImColor(68, 201, 156);
            // case PinType::Float:
            //     return ImColor(147, 226, 74);
            // case PinType::String:
            //     return ImColor(124, 21, 153);
            // case PinType::Object:
            //     return ImColor(51, 150, 215);
            // case PinType::Function:
            //     return ImColor(218, 0, 183);
            // case PinType::Delegate:
            //     return ImColor(255, 48, 48);
        }
    };

    void DrawPinIcon(const std::shared_ptr<port> &pin, bool connected, int alpha)
    {
        IconType iconType;
        ImColor color = GetIconColor(pin->type);
        color.Value.w = alpha / 255.0f;
        switch (pin->type)
        {
        case global_env::value_type::inout_int:
            iconType = IconType::Circle;
            break;
        case global_env::value_type::inout_float:
            iconType = IconType::Circle;
            break;
        case global_env::value_type::inout_bool:
            iconType = IconType::Circle;
            break;
        case global_env::value_type::inout_string:
            iconType = IconType::Circle;
            break;
        case global_env::value_type::inout_image:
            iconType = IconType::Circle;
            break;
        // case PinType::Flow:
        //     iconType = IconType::Flow;
        //     break;
        // case PinType::Bool:
        //     iconType = IconType::Circle;
        //     break;
        // case PinType::Int:
        //     iconType = IconType::Circle;
        //     break;
        // case PinType::Float:
        //     iconType = IconType::Circle;
        //     break;
        // case PinType::String:
        //     iconType = IconType::Circle;
        //     break;
        // case PinType::Object:
        //     iconType = IconType::Circle;
        //     break;
        // case PinType::Function:
        //     iconType = IconType::Circle;
        //     break;
        // case PinType::Delegate:
        //     iconType = IconType::Square;
        //     break;
        default:
            return;
        }

        ax::Widgets::Icon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)), iconType, connected, color, ImColor(32, 32, 32, alpha));
    };

    void ShowLeftPane(float paneWidth)
    {
        auto &io = ImGui::GetIO();

        ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

        paneWidth = ImGui::GetContentRegionAvail().x;

        static bool showStyleEditor = false;
        ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
        ImGui::Spring(0.0f, 0.0f);
        if (ImGui::Button("Zoom to Content"))
            ed::NavigateToContent();
        ImGui::Spring(0.0f);
        if (ImGui::Button("Show Flow"))
        {
            for (auto &link : env->links)
                ed::Flow(link->uuid);
        }
        if (ImGui::Button("run"))
        {
            static int count = 0;
            printf("run count = %d\n", count++);

            Notifier::Add(Notif(Notif::Type::SUCCESS, "run"));
            ExecuteNodes();
        }
        ImGui::Spring();
        if (ImGui::Button("Edit Style"))
            showStyleEditor = true;
        ImGui::EndHorizontal();

        std::vector<ed::NodeId> selectedNodes;
        std::vector<ed::LinkId> selectedLinks;
        selectedNodes.resize(ed::GetSelectedObjectCount());
        selectedLinks.resize(ed::GetSelectedObjectCount());

        int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
        int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

        selectedNodes.resize(nodeCount);
        selectedLinks.resize(linkCount);

        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
            ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::TextUnformatted("Nodes");
        ImGui::Indent();
        for (auto &node : env->nodes)
        {
            ImGui::PushID(node->uuid.AsPointer());
            auto start = ImGui::GetCursorScreenPos();

            bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), node->uuid) != selectedNodes.end();
#if IMGUI_VERSION_NUM >= 18967
            ImGui::SetNextItemAllowOverlap();
#endif
            if (ImGui::Selectable((node->name + "##" + std::to_string(reinterpret_cast<uintptr_t>(node->uuid.AsPointer()))).c_str(), &isSelected))
            {
                if (io.KeyCtrl)
                {
                    if (isSelected)
                        ed::SelectNode(node->uuid, true);
                    else
                        ed::DeselectNode(node->uuid);
                }
                else
                    ed::SelectNode(node->uuid, false);

                ed::NavigateToSelection();
            }

            auto id = std::string("(") + std::to_string(reinterpret_cast<uintptr_t>(node->uuid.AsPointer())) + ")";
            auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);

            auto drawList = ImGui::GetWindowDrawList();
#if IMGUI_VERSION_NUM < 18967
            ImGui::SetItemAllowOverlap();
#else
            ImGui::SetNextItemAllowOverlap();
#endif
            ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
#if IMGUI_VERSION_NUM < 18967
            ImGui::SetItemAllowOverlap();
#else
            ImGui::SetNextItemAllowOverlap();
#endif
            ImGui::SameLine(0, 0);
#if IMGUI_VERSION_NUM < 18967
            ImGui::SetItemAllowOverlap();
#endif

            ImGui::PopID();
        }
        ImGui::Unindent();

        static int changeCount = 0;

        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
            ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::TextUnformatted("Selection");

        ImGui::BeginHorizontal("Selection Stats", ImVec2(paneWidth, 0));
        ImGui::Text("Changed %d time%s", changeCount, changeCount > 1 ? "s" : "");
        ImGui::Spring();
        if (ImGui::Button("Deselect All"))
            ed::ClearSelection();
        ImGui::EndHorizontal();
        ImGui::Indent();
        for (int i = 0; i < nodeCount; ++i)
            ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
        for (int i = 0; i < linkCount; ++i)
            ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
        ImGui::Unindent();

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
            for (auto &link : env->links)
                ed::Flow(link->uuid);

        if (ed::HasSelectionChanged())
            ++changeCount;

        ImGui::EndChild();
    }

    void OnFrame(float deltaTime) override
    {

        auto &io = ImGui::GetIO();

        ImGui::Text("帧率测试: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

        ed::SetCurrentEditor(m_Editor);

        // auto& style = ImGui::GetStyle();

#if 0
        {
            for (auto x = -io.DisplaySize.y; x < io.DisplaySize.x; x += 10.0f)
            {
                ImGui::GetWindowDrawList()->AddLine(ImVec2(x, 0), ImVec2(x + io.DisplaySize.y, io.DisplaySize.y),
                    IM_COL32(255, 255, 0, 255));
            }
        }
#endif

        static ed::NodeId contextNodeId = 0;
        static ed::LinkId contextLinkId = 0;
        static ed::PinId contextPinId = 0;
        static bool createNewNode = false;
        static std::shared_ptr<port> newNodeLinkPin = nullptr;
        static std::shared_ptr<port> newLinkPin = nullptr;

        static float leftPaneWidth = 400.0f;
        static float rightPaneWidth = 800.0f;
        Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

        ShowLeftPane(leftPaneWidth - 4.0f);

        ImGui::SameLine(0.0f, 12.0f);

        ed::Begin("Node editor");
        {
            auto cursorTopLeft = ImGui::GetCursorScreenPos();

            util::BlueprintNodeBuilder builder(m_HeaderBackground, GetTextureWidth(m_HeaderBackground), GetTextureHeight(m_HeaderBackground));

            // 图片节点 绘制
            for (auto &node : env->nodes)
            {
                if (node->group_type != global_env::node_type_group::image_source)
                    continue;

                const auto isSimple = node->group_type == global_env::node_type_group::other;
                bool hasOutputDelegates = false;
                for (auto &output : node->out_ports)
                    if (output->type == global_env::value_type::inout_delegate)
                        hasOutputDelegates = true;

                builder.Begin(node->uuid);
                if (!isSimple)
                {
                    builder.Header(node->ui.color);
                    ImGui::Spring(0);
                    ImGui::TextUnformatted(node->name.c_str());
                    ImGui::Spring(1);
                    ImGui::Dummy(ImVec2(0, 28));
                    if (hasOutputDelegates)
                    {
                        ImGui::BeginVertical("delegates", ImVec2(0, 28));
                        ImGui::Spring(1, 0);
                        for (auto &output : node->out_ports)
                        {
                            if (output->type != global_env::value_type::inout_delegate)
                                continue;

                            auto alpha = ImGui::GetStyle().Alpha;
                            if (newLinkPin && !CanCreateLink(newLinkPin, output) && output != newLinkPin)
                                alpha = alpha * (48.0f / 255.0f);

                            ed::BeginPin(output->uuid, ed::PinKind::Output);
                            ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
                            ed::PinPivotSize(ImVec2(0, 0));
                            ImGui::BeginHorizontal(output->uuid.AsPointer());
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                            if (!output->name.empty())
                            {
                                ImGui::TextUnformatted(output->name.c_str());
                                ImGui::Spring(0);
                            }
                            DrawPinIcon(output, env->is_pin_linked(output->uuid), (int)(alpha * 255));
                            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                            ImGui::EndHorizontal();
                            ImGui::PopStyleVar();
                            ed::EndPin();

                            DrawItemRect(ImColor(255, 0, 0));
                        }
                        ImGui::Spring(1, 0);
                        ImGui::EndVertical();
                        ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                    }
                    else
                        ImGui::Spring(0);
                    builder.EndHeader();
                }
                for (auto &input : node->in_ports)
                {
                    auto alpha = ImGui::GetStyle().Alpha;
                    if (newLinkPin && !CanCreateLink(newLinkPin, input) && input != newLinkPin)
                        alpha = alpha * (48.0f / 255.0f);

                    builder.Input(input->uuid);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                    DrawPinIcon(input, env->is_pin_linked(input->uuid), (int)(alpha * 255));
                    ImGui::Spring(0);
                    if (!input->name.empty())
                    {
                        ImGui::TextUnformatted(input->name.c_str());
                        ImGui::Spring(0);
                    }
                    if (input->type == global_env::value_type::inout_int)
                    {
                        int input_value = 0;
                        int last_value = 0;
                        bool res = input->get(input_value);
                        if (!res)
                            printf("Error: %d\n", input_value);
                        last_value = input_value;
                        ImGui::PushItemWidth(100.0f);
                        ImGui::InputInt("##edit", &input_value);
                        ImGui::PopItemWidth();
                        if (last_value != input_value)
                        {
                            printf("input_value = %d\n", input_value);
                            input->set_self(input_value);
                        }
                        ImGui::Spring(0);
                    }
                    if (input->type == global_env::value_type::inout_float)
                    {
                        float input_value = 0.0f;
                        float last_value = 0.0f;
                        bool res = input->get(input_value);
                        if (!res)
                            printf("Error: %f\n", input_value);
                        last_value = input_value;
                        ImGui::PushItemWidth(100.0f);
                        ImGui::InputFloat("##edit", &input_value);
                        ImGui::PopItemWidth();
                        if (last_value != input_value)
                        {
                            printf("input_value = %f\n", input_value);
                            input->set_self(input_value);
                        }
                        ImGui::Spring(0);
                    }
                    if (input->type == global_env::value_type::inout_bool)
                    {
                        bool input_value = false;
                        bool last_value = false;
                        bool res = input->get(input_value);
                        if (!res)
                            printf("Error: %d\n", input_value);
                        last_value = input_value;
                        ImGui::PushItemWidth(100.0f);
                        ImGui::Checkbox("##edit", &input_value);
                        ImGui::PopItemWidth();
                        if (last_value != input_value)
                        {
                            printf("input_value = %d\n", input_value);
                            input->set_self(input_value);
                        }
                        ImGui::Spring(0);
                    }
                    if (input->type == global_env::value_type::inout_string)
                    {
                        std::string input_value;
                        bool res = input->get(input_value);
                        if (!res)
                            printf("Error: %s\n", input_value.c_str());
                        char buffer[128] = {0};
                        std::copy(input_value.begin(), input_value.end(), buffer);
                        ImGui::PushItemWidth(100.0f);
                        ImGui::InputText("##edit", buffer, 127);
                        ImGui::PopItemWidth();
                        if (buffer != input_value)
                        {
                            input->set_self(std::string(buffer));
                        }
                        ImGui::Spring(0);
                    }
                    if (input->type == global_env::value_type::inout_image)
                    {
                        // if (input->has_image())
                        //{
                        //     if (input->image_texture == nullptr)
                        //         input->gen_image_texture(this);
                        //     ImGui::Image((void *)(intptr_t)input->image_texture, ImVec2(100, 100));
                        //
                        // }

                        ImGui::Spring(0);
                    }

                    ImGui::PopStyleVar();
                    builder.EndInput();
                }

                for (auto &output : node->out_ports)
                {
                    // if (!isSimple)
                    //     continue;

                    auto alpha = ImGui::GetStyle().Alpha;
                    if (newLinkPin && !CanCreateLink(newLinkPin, output) && output != newLinkPin)
                        alpha = alpha * (48.0f / 255.0f);

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                    builder.Output(output->uuid);
                    if (output->type == global_env::value_type::inout_int)
                    {
                        int output_value = 0;
                        bool res = output->get(output_value);
                        if (!res)
                            printf("Error: %d\n", output_value);
                        ImGui::Text("%d", output_value);

                        ImGui::Spring(0);
                    }
                    if (output->type == global_env::value_type::inout_float)
                    {
                        float output_value = 0.0f;
                        bool res = output->get(output_value);
                        if (!res)
                            printf("Error: %f\n", output_value);
                        ImGui::Text("%f", output_value);

                        ImGui::Spring(0);
                    }
                    if (output->type == global_env::value_type::inout_bool)
                    {
                        bool output_value = false;
                        bool res = output->get(output_value);
                        if (!res)
                            printf("Error: %d\n", output_value);
                        ImGui::Text("%d", output_value);

                        ImGui::Spring(0);
                    }
                    if (output->type == global_env::value_type::inout_string)
                    {
                        std::string output_value;
                        bool res = output->get(output_value);
                        if (!res)
                            printf("Error: %s\n", output_value.c_str());
                        ImGui::Text("%s", output_value.c_str());

                        ImGui::Spring(0);
                    }
                    if (output->type == global_env::value_type::inout_image)
                    {
                        // if (output->has_image())
                        //{
                        //     if (output->image_texture == nullptr)
                        //         output->gen_image_texture(this);
                        //     ImGui::Image((void *)(intptr_t)output->image_texture, ImVec2(100, 100));
                        //
                        // }

                        ImGui::Spring(0);
                    }

                    if (!output->name.empty())
                    {
                        ImGui::Spring(0);
                        ImGui::TextUnformatted(output->name.c_str());
                    }
                    ImGui::Spring(0);
                    DrawPinIcon(output, env->is_pin_linked(output->uuid), (int)(alpha * 255));
                    ImGui::PopStyleVar();
                    builder.EndOutput();
                }

                builder.End();
            }

            for (auto &link : env->links)
                ed::Link(link->uuid, link->which_from_out_port.lock()->uuid, link->which_to_in_port.lock()->uuid, link->ui.color, 2.0f);

            if (!createNewNode)
            {
                if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
                {
                    auto showLabel = [](const char *label, ImColor color)
                    {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                        auto size = ImGui::CalcTextSize(label);

                        auto padding = ImGui::GetStyle().FramePadding;
                        auto spacing = ImGui::GetStyle().ItemSpacing;

                        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                        auto rectMin = ImGui::GetCursorScreenPos() - padding;
                        auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                        auto drawList = ImGui::GetWindowDrawList();
                        drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                        ImGui::TextUnformatted(label);
                    };

                    ed::PinId startPinId = 0, endPinId = 0;
                    if (ed::QueryNewLink(&startPinId, &endPinId))
                    {
                        auto startPin = env->find_port(startPinId);
                        auto endPin = env->find_port(endPinId);

                        newLinkPin = startPin ? startPin : endPin;
                        if (startPin->control_type == port::port_type::in)
                        {
                            startPin.swap(endPin);
                        }
                        auto from_ptr = std::dynamic_pointer_cast<out_port>(startPin);
                        auto to_ptr = std::dynamic_pointer_cast<in_port>(endPin);

                        if (from_ptr && to_ptr)
                        {
                            if (to_ptr->uuid == from_ptr->uuid)
                            {
                                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                            }
                            else if (to_ptr->control_type == port::port_type::in)
                            {
                                showLabel("x Cannot connect to input", ImColor(45, 32, 32, 180));
                                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                            }
                            // else if (endPin->Node == startPin->Node)
                            //{
                            //     showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            //     ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                            // }
                            else if (to_ptr->type != from_ptr->type)
                            {
                                showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                                ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                            }
                            else
                            {
                                showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                                if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                                {
                                    std::string name = "link";
                                    std::string label = env->enum_to_string(from_ptr->type) + " to " + env->enum_to_string(to_ptr->type);
                                    auto link_ptr = std::make_shared<link>(env->gen_next_link_id(), name, label, to_ptr, from_ptr);
                                    link_ptr->ui.color = GetIconColor(from_ptr->type);
                                    env->links.push_back(link_ptr);
                                }
                            }
                        }
                    }

                    ed::PinId pinId = 0;
                    if (ed::QueryNewNode(&pinId))
                    {
                        newLinkPin = env->find_port(pinId);
                        if (newLinkPin)
                            showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                        if (ed::AcceptNewItem())
                        {
                            createNewNode = true;
                            newNodeLinkPin = env->find_port(pinId);
                            newLinkPin = nullptr;
                            ed::Suspend();
                            ImGui::OpenPopup("Create New Node");
                            ed::Resume();
                        }
                    }
                }
                else
                    newLinkPin = nullptr;

                ed::EndCreate();

                if (ed::BeginDelete())
                {
                    ed::NodeId nodeId = 0;
                    while (ed::QueryDeletedNode(&nodeId))
                    {
                        if (ed::AcceptDeletedItem())
                        {
                            auto id = std::find_if(env->nodes.begin(), env->nodes.end(), [nodeId](auto &node)
                                                   { return node->uuid == nodeId; });
                            if (id != env->nodes.end())
                                env->nodes.erase(id);
                        }
                    }

                    ed::LinkId linkId = 0;
                    while (ed::QueryDeletedLink(&linkId))
                    {
                        if (ed::AcceptDeletedItem())
                        {
                            auto id = std::find_if(env->links.begin(), env->links.end(), [linkId](auto &link)
                                                   { return link->uuid == linkId; });
                            if (id != env->links.end())
                                env->links.erase(id);
                        }
                    }
                }
                ed::EndDelete();
            }

            ImGui::SetCursorScreenPos(cursorTopLeft);
        }

#if 1
        auto openPopupPosition = ImGui::GetMousePos();
        ed::Suspend();
        if (ed::ShowNodeContextMenu(&contextNodeId))
            ImGui::OpenPopup("Node Context Menu");
        else if (ed::ShowPinContextMenu(&contextPinId))
            ImGui::OpenPopup("Pin Context Menu");
        else if (ed::ShowLinkContextMenu(&contextLinkId))
            ImGui::OpenPopup("Link Context Menu");
        else if (ed::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("Create New Node");
            newNodeLinkPin = nullptr;
        }
        ed::Resume();

        ed::Suspend();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        if (ImGui::BeginPopup("Node Context Menu"))
        {
            auto node = env->find_node(contextNodeId);

            ImGui::TextUnformatted("Node Context Menu");
            ImGui::Separator();
            if (node)
            {
                ImGui::Text("ID: %p", node->uuid.AsPointer());
                ImGui::Text("Type: %s", env->enum_to_string(node->group_type).c_str());
                ImGui::Text("Inputs: %d", (int)node->in_ports.size());
                ImGui::Text("Outputs: %d", (int)node->out_ports.size());
            }
            else
                ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
            ImGui::Separator();
            if (ImGui::MenuItem("Delete"))
                ed::DeleteNode(contextNodeId);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Pin Context Menu"))
        {
            auto pin = env->find_port(contextPinId);

            ImGui::TextUnformatted("Pin Context Menu");
            ImGui::Separator();
            if (pin)
            {
                ImGui::Text("ID: %p", pin->uuid.AsPointer());
                if (pin->as_node_ptr.expired() == false)
                    ImGui::Text("Node: %p", pin->as_node_ptr.lock()->uuid.AsPointer());
                else
                    ImGui::Text("Node: %s", "<none>");
            }
            else
                ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Link Context Menu"))
        {
            auto link = env->find_link(contextLinkId);

            ImGui::TextUnformatted("Link Context Menu");
            ImGui::Separator();
            if (link)
            {
                ImGui::Text("ID: %p", link->uuid.AsPointer());
                ImGui::Text("From: %p", link->which_from_out_port.lock()->uuid.AsPointer());
                ImGui::Text("To: %p", link->which_to_in_port.lock()->uuid.AsPointer());
            }
            else
                ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
            ImGui::Separator();
            if (ImGui::MenuItem("Delete"))
                ed::DeleteLink(contextLinkId);
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Create New Node"))
        {
            auto newNodePostion = openPopupPosition;
            // ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

            // auto drawList = ImGui::GetWindowDrawList();
            // drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

            // 这里是右键创建新的节点，走一个遍历所有节点列表然后创建新的节点
            std::shared_ptr<base_node> node = nullptr;
            for (auto &[type, type_name] : global_env::node_type_group_map)
            {
                for (auto &[name, factory] : global_env::node_builder_factories[type])
                {
                    if (ImGui::MenuItem(name.c_str()))
                    {
                        node = factory(env->get_base_ptr());
                    }
                }
                if (type != env->node_type_group_map.rbegin()->first)
                    ImGui::Separator();
            }

            if (node)
            {
                createNewNode = false;

                ed::SetNodePosition(node->uuid, newNodePostion);

                env->nodes.push_back(node);

                // if (auto startPin = newNodeLinkPin)
                //{
                //     auto &pins = startPin->control_type == port::port_type::in ? node->out_ports : node->in_ports;

                //    for (auto &pin : pins)
                //    {
                //        if (CanCreateLink(startPin, pin))
                //        {
                //            std::shared_ptr<port> endPin = pin;
                //            if (startPin->control_type == port::port_type::in)
                //                startPin.swap(endPin);
                //            // 创建新的连接
                //            // m_Graph.Links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                //            auto link_ptr = std::make_shared<link>(env->gen_next_link_id(), "link", "link", endPin, startPin);
                //            link_ptr->ui.color = GetIconColor(startPin->type);
                //            env->links.push_back(link_ptr);
                //            break;
                //        }
                //    }
                //}
            }

            ImGui::EndPopup();
        }
        else
            createNewNode = false;
        ImGui::PopStyleVar();
        ed::Resume();
#endif

        ed::End();

        auto editorMin = ImGui::GetItemRectMin();
        auto editorMax = ImGui::GetItemRectMax();

        if (m_ShowOrdinals)
        {
            int nodeCount = ed::GetNodeCount();
            std::vector<ed::NodeId> orderedNodeIds;
            orderedNodeIds.resize(static_cast<size_t>(nodeCount));
            ed::GetOrderedNodeIds(orderedNodeIds.data(), nodeCount);

            auto drawList = ImGui::GetWindowDrawList();
            drawList->PushClipRect(editorMin, editorMax);

            int ordinal = 0;
            for (auto &nodeId : orderedNodeIds)
            {
                auto p0 = ed::GetNodePosition(nodeId);
                auto p1 = p0 + ed::GetNodeSize(nodeId);
                p0 = ed::CanvasToScreen(p0);
                p1 = ed::CanvasToScreen(p1);

                ImGuiTextBuffer builder;
                builder.appendf("#%d", ordinal++);

                auto textSize = ImGui::CalcTextSize(builder.c_str());
                auto padding = ImVec2(2.0f, 2.0f);
                auto widgetSize = textSize + padding * 2;

                auto widgetPosition = ImVec2(p1.x, p0.y) + ImVec2(0.0f, -widgetSize.y);

                drawList->AddRectFilled(widgetPosition, widgetPosition + widgetSize, IM_COL32(100, 80, 80, 190), 3.0f, ImDrawFlags_RoundCornersAll);
                drawList->AddRect(widgetPosition, widgetPosition + widgetSize, IM_COL32(200, 160, 160, 190), 3.0f, ImDrawFlags_RoundCornersAll);
                drawList->AddText(widgetPosition + padding, IM_COL32(255, 255, 255, 255), builder.c_str());
            }

            drawList->PopClipRect();
        }
    }

    int m_NextId = 1;
    const int m_PinIconSize = 24;

    ImTextureID m_HeaderBackground = nullptr;
    ImTextureID m_SaveIcon = nullptr;
    ImTextureID m_RestoreIcon = nullptr;
    bool m_ShowOrdinals = false;

    std::shared_ptr<image_flow_env> env = std::make_shared<image_flow_env>();
};

int main(int argc, char **argv)
{
    Example exampe("Blueprints", argc, argv);

    if (exampe.Create())
        return exampe.Run();

    return 0;
}