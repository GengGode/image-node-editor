#define IMGUI_DEFINE_MATH_OPERATORS
#include <application.h>
#include "notifiers/Notifier.hpp"
#include "utilities/builders.h"
#include "utilities/widgets.h"
#include "utilities/minidump.h"

#include "addons/ImFileDialog.h"
#include "addons/imspinner.h"
#include "../application/addons/ImGuiNotify.hpp"

#include <imgui_tex_inspect.h>
#include <imgui_node_editor.h>
#include <imgui_internal.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <functional>
#include <memory>
#include <set>

#include "nodes/base_nodes.hpp"
#include "nodes/node_ui_colors.hpp"
#include "nodes/graph_ui.hpp"

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

    void TouchNode(ed::NodeId id)
    {
        m_NodeTouchTime[id] = m_TouchTime;
    }

    float GetTouchProgress(ed::NodeId id)
    {
        auto it = m_NodeTouchTime.find(id);
        if (it != m_NodeTouchTime.end() && it->second > 0.0f)
            return (m_TouchTime - it->second) / m_TouchTime;
        else
            return 0.0f;
    }

    void UpdateTouch()
    {
        const auto deltaTime = ImGui::GetIO().DeltaTime;
        for (auto &entry : m_NodeTouchTime)
        {
            if (entry.second > 0.0f)
                entry.second -= deltaTime;
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

            auto node = self->m_Graph.FindNode(nodeId);
            if (!node)
                return 0;

            if (data != nullptr)
                memcpy(data, node->State.data(), node->State.size());
            return node->State.size();
        };

        config.SaveNodeSettings = [](ed::NodeId nodeId, const char *data, size_t size, ed::SaveReasonFlags reason, void *userPointer) -> bool
        {
            auto self = static_cast<Example *>(userPointer);

            auto node = self->m_Graph.FindNode(nodeId);
            if (!node)
                return false;

            node->State.assign(data, size);

            self->TouchNode(nodeId);

            return true;
        };

        m_Editor = ed::CreateEditor(&config);
        ed::SetCurrentEditor(m_Editor);
        ed::NavigateToContent();

        m_Graph.build_nodes();

        m_HeaderBackground = LoadTexture("data/BlueprintBackground.png");
        m_SaveIcon = LoadTexture("data/ic_save_white_24dp.png");
        m_RestoreIcon = LoadTexture("data/ic_restore_white_24dp.png");
        m_PlayIcon = LoadTexture("data/ic_play_arrow_white_24dp.png");

        // graph env init
        m_Graph.ui.graph = &m_Graph;
        m_Graph.env.app = this;
        m_Graph.env.graph = &m_Graph;
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

        m_Graph.env.need_stop();

        releaseTexture(m_RestoreIcon);
        releaseTexture(m_SaveIcon);
        releaseTexture(m_HeaderBackground);
        releaseTexture(m_PlayIcon);

        if (m_Editor)
        {
            ed::DestroyEditor(m_Editor);
            m_Editor = nullptr;
        }
    }

    void ShowStyleEditor(bool *show = nullptr)
    {
        if (!ImGui::Begin("Style", show))
        {
            ImGui::End();
            return;
        }

        auto paneWidth = ImGui::GetContentRegionAvail().x;

        auto &editorStyle = ed::GetStyle();
        ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth, 0), 1.0f);
        ImGui::TextUnformatted("Values");
        ImGui::Spring();
        if (ImGui::Button("Reset to defaults"))
            editorStyle = ed::Style();
        ImGui::EndHorizontal();
        ImGui::Spacing();
        ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Hovered Node Border Offset", &editorStyle.HoverNodeBorderOffset, 0.1f, -40.0f, 40.0f);
        ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Selected Node Border Offset", &editorStyle.SelectedNodeBorderOffset, 0.1f, -40.0f, 40.0f);
        ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
        ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
        // ImVec2  SourceDirection;
        // ImVec2  TargetDirection;
        ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
        ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
        ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
        ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
        // ImVec2  PivotAlignment;
        // ImVec2  PivotSize;
        // ImVec2  PivotScale;
        // float   PinCorners;
        // float   PinRadius;
        // float   PinArrowSize;
        // float   PinArrowWidth;
        ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
        ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

        ImGui::Separator();

        static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_DisplayRGB;
        ImGui::BeginHorizontal("Color Mode", ImVec2(paneWidth, 0), 1.0f);
        ImGui::TextUnformatted("Filter Colors");
        ImGui::Spring();
        ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_DisplayRGB);
        ImGui::Spring(0);
        ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_DisplayHSV);
        ImGui::Spring(0);
        ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);
        ImGui::EndHorizontal();

        static ImGuiTextFilter filter;
        filter.Draw("##filter", paneWidth);

        ImGui::Spacing();

        ImGui::PushItemWidth(-160);
        for (int i = 0; i < ed::StyleColor_Count; ++i)
        {
            auto name = ed::GetStyleColorName((ed::StyleColor)i);
            if (!filter.PassFilter(name))
                continue;

            ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
        }
        ImGui::PopItemWidth();

        ImGui::End();
    }

    void ShowLeftPane(float paneWidth)
    {
        auto &io = ImGui::GetIO();

        ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

        paneWidth = ImGui::GetContentRegionAvail().x;

        static bool showStyleEditor = false;
        ImGui::BeginHorizontal("Run", ImVec2(paneWidth, 0));
        ImGui::Spring(0.0f, 0.0f);
        if (ImGui::Button("运行"))
        {
            static int count = 0;

            m_Graph.env.need_execute();
            Notifier::Add(Notif(Notif::Type::INFO, "运行", "运行次数: " + std::to_string(count++),10));
        }
        ImGui::Spring(0.0f);
        if (ImGui::Button("清空"))
        {
            m_Graph.Nodes.clear();
            m_Graph.Links.clear();
            m_Graph.next_id = 0;
            ImGui::InsertNotification({ImGuiToastType::Info, 3000, "清空所有节点"});
        }
        static bool loop_execute = false;
        ImGui::Checkbox("循环执行", &loop_execute);
        if (loop_execute)
        {
            static int count = 0;
            m_Graph.env.need_execute();
            printf("循环执行次数: %d\n", count++);
        }
        if(ImGui::Button("自动排列"))
        {
            m_Graph.auto_arrange();
        }
        ImGui::Spring();
        if (ImGui::Button("序列化"))
        {
            for (auto &node : m_Graph.Nodes)
            {
                node.Position = ed::GetNodePosition(node.ID);
            }

            std::string json;
            m_Graph.serialize(json);
            printf("json = %s\n", json.c_str());
            std::ofstream out("data/project.json");
            out << json;
            out.close();
        }
        if (ImGui::Button("反序列化"))
        {
            std::ifstream in("data/project.json");
            std::string json((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
            in.close();

            m_Graph.deserialize(json);
            m_Graph.build_nodes();
            for (auto &node : m_Graph.Nodes)
            {
                ed::SetNodePosition(node.ID, node.Position);
            }
        }
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
        ImGui::Spring(0.0f, 0.0f);
        if (ImGui::Button("居中展示"))
            ed::NavigateToContent();
        ImGui::Spring(0.0f);
        if (ImGui::Button("显示流"))
        {
            for (auto &link : m_Graph.Links)
                ed::Flow(link.ID);
        }
        ImGui::Spring();
        if (ImGui::Button("Edit Style"))
            showStyleEditor = true;
        ImGui::EndHorizontal();
        ImGui::Checkbox("Show Ordinals", &m_ShowOrdinals);

        if (showStyleEditor)
            ShowStyleEditor(&showStyleEditor);

        std::vector<ed::NodeId> selectedNodes;
        std::vector<ed::LinkId> selectedLinks;
        selectedNodes.resize(ed::GetSelectedObjectCount());
        selectedLinks.resize(ed::GetSelectedObjectCount());

        int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
        int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

        selectedNodes.resize(nodeCount);
        selectedLinks.resize(linkCount);

        int saveIconWidth = GetTextureWidth(m_SaveIcon);
        int saveIconHeight = GetTextureWidth(m_SaveIcon);
        int restoreIconWidth = GetTextureWidth(m_RestoreIcon);
        int restoreIconHeight = GetTextureWidth(m_RestoreIcon);

        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
            ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::TextUnformatted("Nodes");
        ImGui::Indent();
        for (auto &node : m_Graph.Nodes)
        {
            ImGui::PushID(node.ID.AsPointer());
            auto start = ImGui::GetCursorScreenPos();

            if (const auto progress = GetTouchProgress(node.ID))
            {
                ImGui::GetWindowDrawList()->AddLine(
                    start + ImVec2(-8, 0),
                    start + ImVec2(-8, ImGui::GetTextLineHeight()),
                    IM_COL32(255, 0, 0, 255 - (int)(255 * progress)), 4.0f);
            }

            bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), node.ID) != selectedNodes.end();

            if (ImGui::Selectable((node.Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer()))).c_str(), &isSelected))
            {
                if (io.KeyCtrl)
                {
                    if (isSelected)
                        ed::SelectNode(node.ID, true);
                    else
                        ed::DeselectNode(node.ID);
                }
                else
                    ed::SelectNode(node.ID, false);

                ed::NavigateToSelection();
            }
            if (ImGui::IsItemHovered() && !node.State.empty())
                ImGui::SetTooltip("State: %s", node.State.c_str());

            auto id = std::string("(") + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer())) + ")";
            auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);
            auto iconPanelPos = start + ImVec2(
                                            paneWidth - ImGui::GetStyle().FramePadding.x - ImGui::GetStyle().IndentSpacing - saveIconWidth - restoreIconWidth - ImGui::GetStyle().ItemInnerSpacing.x * 1,
                                            (ImGui::GetTextLineHeight() - saveIconHeight) / 2);
            ImGui::GetWindowDrawList()->AddText(
                ImVec2(iconPanelPos.x - textSize.x - ImGui::GetStyle().ItemInnerSpacing.x, start.y),
                IM_COL32(255, 255, 255, 255), id.c_str(), nullptr);

            std::string node_run_time = "";
            if (node.LastExecuteResult.has_error())
            {
                node_run_time = "error";
            }
            else
            {
                if (node.ExecuteTime.has_value())
                {
                    node_run_time = std::to_string(node.ExecuteTime.value().count() / 1000000.0);
                    node_run_time = node_run_time.substr(0, node_run_time.find(".") + 3) + "ms";
                }
            }
            if (node_run_time.empty() == false)
            {
                auto run_time_text_size = ImGui::CalcTextSize(node_run_time.c_str(), nullptr);
                ImGui::GetWindowDrawList()->AddText(
                    ImVec2(iconPanelPos.x - run_time_text_size.x - ImGui::GetStyle().ItemInnerSpacing.x - textSize.x - ImGui::GetStyle().ItemInnerSpacing.x, start.y),
                    IM_COL32(255, 255, 255, 255), node_run_time.c_str(), nullptr);
            }

            auto drawList = ImGui::GetWindowDrawList();
            ImGui::SetCursorScreenPos(iconPanelPos);

            ImGui::SetItemAllowOverlap();
            if (node.SavedState.empty())
            {
                if (ImGui::InvisibleButton("save", ImVec2((float)saveIconWidth, (float)saveIconHeight)))
                    node.SavedState = node.State;

                if (ImGui::IsItemActive())
                    drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
                else if (ImGui::IsItemHovered())
                    drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                else
                    drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
            }
            else
            {
                ImGui::Dummy(ImVec2((float)saveIconWidth, (float)saveIconHeight));
                drawList->AddImage(m_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
            }

            ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

            ImGui::SetItemAllowOverlap();
            if (!node.SavedState.empty())
            {
                if (ImGui::InvisibleButton("restore", ImVec2((float)restoreIconWidth, (float)restoreIconHeight)))
                {
                    node.State = node.SavedState;
                    ed::RestoreNodeState(node.ID);
                    node.SavedState.clear();
                }

                if (ImGui::IsItemActive())
                    drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
                else if (ImGui::IsItemHovered())
                    drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
                else
                    drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
            }
            else
            {
                ImGui::Dummy(ImVec2((float)restoreIconWidth, (float)restoreIconHeight));
                drawList->AddImage(m_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
            }

            ImGui::SameLine(0, 0);

            ImGui::SetItemAllowOverlap();
            ImGui::Dummy(ImVec2(0, (float)restoreIconHeight));

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
            for (auto &link : m_Graph.Links)
                ed::Flow(link.ID);

        if (ed::HasSelectionChanged())
            ++changeCount;

        ImGui::EndChild();
    }

    void OnFrame(float deltaTime) override
    {
        UpdateTouch();

        auto &io = ImGui::GetIO();

        ImGui::Text("帧率测试: %.2f (%.2gms) 上次执行全体耗时: %.2f ms", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f, m_Graph.env.all_execute_time / 1000000.0);

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
        static Pin *newNodeLinkPin = nullptr;
        //static Pin *new_link_pin = nullptr;
        static bool isUpdateNewNodePositon = false;

        static float leftPaneWidth = 400.0f;
        static float rightPaneWidth = 800.0f;

        Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

        ShowLeftPane(leftPaneWidth - 4.0f);

        ImGui::SameLine(0.0f, 12.0f);

        ed::Begin("Node editor");
        {
            auto cursorTopLeft = ImGui::GetCursorScreenPos();

            util::BlueprintNodeBuilder builder(m_HeaderBackground, GetTextureWidth(m_HeaderBackground), GetTextureHeight(m_HeaderBackground));

            m_Graph.ui.builder = &builder;
            m_Graph.ui.draw_image_nodes();
            m_Graph.ui.draw_comment_nodes();
            m_Graph.ui.draw_links();
            m_Graph.ui.draw_virtual_links();

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
                        auto startPin = m_Graph.FindPin(startPinId);
                        auto endPin = m_Graph.FindPin(endPinId);

                        m_Graph.ui.new_link_pin = startPin ? startPin : endPin;

                        if (startPin->Kind == PinKind::Input)
                        {
                            std::swap(startPin, endPin);
                            std::swap(startPinId, endPinId);
                        }

                        if (startPin && endPin)
                        {
                            auto convert_factory_it = NodeWorldGlobal::registerLinkAutoConvertNodeFactories.find({startPin->Type, endPin->Type});
                            auto has_convertor = (convert_factory_it != NodeWorldGlobal::registerLinkAutoConvertNodeFactories.end());
                            if (endPin == startPin)
                            {
                                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                            }
                            else if (endPin->Kind == startPin->Kind)
                            {
                                showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                            }
                            else if (endPin->Node == startPin->Node)
                            {
                                showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                                ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                            }
                            else if (endPin->Type != startPin->Type && !has_convertor)
                            {
                                showLabel("x Incompatible Pin Type Or Not Find Converter", ImColor(45, 32, 32, 180));
                                ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                            }
                            else
                            {
                                showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                                if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                                {
                                    if (has_convertor)
                                    {
                                        auto convert_factory = convert_factory_it->second;
                                        auto convert_node = convert_factory([&]()
                                                                            { return GetNextId(); }, [&](Node *node)
                                                                            { m_Graph.build_node(node); }, m_Graph.Nodes, this);
                                        // 计算位置
                                        m_Graph.build_nodes();
                                        m_Graph.Links.emplace_back(Link(GetNextId(), startPinId, convert_node->Inputs[0].ID));
                                        m_Graph.Links.emplace_back(Link(GetNextId(), convert_node->Outputs[0].ID, endPinId));
                                        m_Graph.Links.back().Color = ui::GetIconColor(startPin->Type);
                                        m_Graph.Links.back().Color = ui::GetIconColor(endPin->Type);
                                        startPin = nullptr;
                                        endPin = nullptr;
                                    }
                                    else
                                    {
                                        // 创建新的连接
                                        m_Graph.Links.emplace_back(Link(m_Graph.get_next_id(), startPin->ID, endPin->ID));
                                        m_Graph.Links.back().Color = ui::GetIconColor(startPin->Type);
                                    }
                                }
                            }
                        }
                    }

                    ed::PinId pinId = 0;
                    if (ed::QueryNewNode(&pinId))
                    {
                        m_Graph.ui.new_link_pin = m_Graph.FindPin(pinId);
                        if (m_Graph.ui.new_link_pin)
                            showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                        if (ed::AcceptNewItem())
                        {
                            createNewNode = true;
                            newNodeLinkPin = m_Graph.FindPin(pinId);
                            m_Graph.ui.new_link_pin = nullptr;
                            ed::Suspend();
                            ImGui::OpenPopup("Create New Node");
                            isUpdateNewNodePositon = true;
                            ed::Resume();
                        }
                    }
                }
                else
                    m_Graph.ui.new_link_pin = nullptr;

                ed::EndCreate();

                if (ed::BeginDelete())
                {
                    ed::NodeId nodeId = 0;
                    while (ed::QueryDeletedNode(&nodeId))
                    {
                        if (ed::AcceptDeletedItem())
                        {
                            auto id = std::find_if(m_Graph.Nodes.begin(), m_Graph.Nodes.end(), [nodeId](auto &node)
                                                   { return node.ID == nodeId; });
                            if (id != m_Graph.Nodes.end())
                                m_Graph.Nodes.erase(id);
                        }
                    }

                    ed::LinkId linkId = 0;
                    while (ed::QueryDeletedLink(&linkId))
                    {
                        if (ed::AcceptDeletedItem())
                        {
                            auto id = std::find_if(m_Graph.Links.begin(), m_Graph.Links.end(), [linkId](auto &link)
                                                   { return link.ID == linkId; });
                            if (id != m_Graph.Links.end())
                                m_Graph.Links.erase(id);
                        }
                    }
                }
                ed::EndDelete();
            }

            ImGui::SetCursorScreenPos(cursorTopLeft);
        }

#if 1
        auto openPopupPosition = ImGui::GetMousePos();
        static ImVec2 createNodePosition;
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
            isUpdateNewNodePositon = true;
        }
        if (isUpdateNewNodePositon)
        {
            createNodePosition = openPopupPosition;
            isUpdateNewNodePositon = false;
        }
        ed::Resume();

        ed::Suspend();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        if (ImGui::BeginPopup("Node Context Menu"))
        {
            auto node = m_Graph.FindNode(contextNodeId);

            ImGui::TextUnformatted("Node Context Menu");
            if (node)
            {
                ImGui::Separator();
                if (ImGui::MenuItem("执行"))
                {
                    async_execute_node(node);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("折叠"))
                {
                    if(node->ui.is_expanded)
                        node->collapse();
                    else
                        node->expand();
                }
                ImGui::Separator();
                ImGui::Text("ID: %p", node->ID.AsPointer());
                ImGui::Text("Type: %s", node->Type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Simple ? "Simple" : "Comment"));
                ImGui::Text("Inputs: %d", (int)node->Inputs.size());
                ImGui::Text("Outputs: %d", (int)node->Outputs.size());
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
            auto pin = m_Graph.FindPin(contextPinId);

            ImGui::TextUnformatted("Pin Context Menu");
            ImGui::Separator();
            if (pin)
            {
                ImGui::Text("ID: %p", pin->ID.AsPointer());
                if (pin->Node)
                    ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
                else
                    ImGui::Text("Node: %s", "<none>");
            }
            else
                ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Link Context Menu"))
        {
            auto link = m_Graph.FindLink(contextLinkId);

            ImGui::TextUnformatted("Link Context Menu");
            ImGui::Separator();
            if (link)
            {
                ImGui::Text("ID: %p", link->ID.AsPointer());
                ImGui::Text("From: %p", link->StartPinID.AsPointer());
                ImGui::Text("To: %p", link->EndPinID.AsPointer());
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
            Node *node = nullptr;
            for (auto &[type_name, type] : nodeTypes)
            {
                if (ImGui::BeginMenu(type_name.c_str()))
                {
                    for (auto &[name, func] : NodeWorldGlobal::nodeFactories[type])
                    {
                        if (ImGui::MenuItem(name.c_str()))
                        {
                            node = func([&]()
                                        { return m_Graph.get_next_id(); },
                                        [&](Node *node)
                                        { m_Graph.build_node(node); },
                                        m_Graph.Nodes, this);
                        }
                    }
                    ImGui::EndMenu();
                }
            }

            if (node)
            {
                m_Graph.build_nodes();

                createNewNode = false;

                ed::SetNodePosition(node->ID, createNodePosition);

                if (auto startPin = newNodeLinkPin)
                {
                    auto &pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                    for (auto &pin : pins)
                    {
                        if (CanCreateLink(startPin, &pin))
                        {
                            auto endPin = &pin;
                            if (startPin->Kind == PinKind::Input)
                                std::swap(startPin, endPin);
                            // 创建新的连接
                            m_Graph.Links.emplace_back(Link(m_Graph.get_next_id(), startPin->ID, endPin->ID));
                            m_Graph.Links.back().Color = ui::GetIconColor(startPin->Type);
                            break;
                        }
                    }
                }
            }

            ImGui::EndPopup();
        }
        else
            createNewNode = false;
        ImGui::PopStyleVar();
        ed::Resume();
#endif

        ed::End();

        Node *output_node = nullptr;
        for (auto &node : m_Graph.Nodes)
            if (node.Name == "图像查看器")
            {
                std::string name = "output " + std::to_string((int)(size_t)node.ID);
                ImGui::Begin(name.c_str());
                output_node = &node;
                if (output_node != nullptr)
                {
                    auto input = &output_node->Inputs[0];
                    if (input->HasImage())
                    {
                        auto size = ImGui::GetContentRegionAvail();
                        auto image_size = std::get<cv::Mat>(input->Value).size();
                        auto texture_size = ImVec2(static_cast<float>(image_size.width), static_cast<float>(image_size.height));
                        ImGuiTexInspect::BeginInspectorPanel("Inspector", input->ImageTexture, texture_size, ImGuiTexInspect::InspectorFlags_NoGrid, ImGuiTexInspect::SizeExcludingBorder(ImVec2(size.x - 2, size.y - 2)));
                        ImGuiTexInspect::DrawAnnotations(ImGuiTexInspect::ValueText(ImGuiTexInspect::ValueText::BytesDec));
                        ImGuiTexInspect::EndInspectorPanel();
                    }
                }
                ImGui::End();
            }

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

        static bool show_profiler_window = true;
        // if (show_profiler_window)
        //{
        //     ImGui::Begin("Profiler Window", &show_profiler_window);
        //     auto &entry = profiler._entries[profiler.GetCurrentEntryIndex()];
        //     ImGuiWidgetFlameGraph::PlotFlame("CPU", &ProfilerValueGetter, &entry, Profiler::_StageCount, 0, "Main Thread", FLT_MAX, FLT_MAX, ImVec2(400, 0));
        //     ImGui::End();
        // }

        // ImGui::PushFont(io.Fonts->Fonts[1]);
        // ImGui::PopFont();

        // ImGui::ShowMetricsWindow();
        ImGui::Begin("Test File Dialog");
        if (ImGui::Button("Open a texture"))
            ifd::FileDialog::Instance().Open("TextureOpenDialog", "Open a texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*");
        if (ifd::FileDialog::Instance().IsDone("TextureOpenDialog"))
        {
            if (ifd::FileDialog::Instance().HasResult())
            {
                std::string res = ifd::FileDialog::Instance().GetResult().u8string();
                printf("OPEN[%s]\n", res.c_str());
            }
            ifd::FileDialog::Instance().Close();
        }
        ImGui::End();

        //--------------------------
        // 右下角弹出通知
        Notifier::Draw(); 

        //---------------------------
        // 循环清理异步任务
        try_clear_futures();
    }

    std::list<std::future<void>> node_execute_futures;

    void async_execute_node(Node *node)
    {
        node_execute_futures.emplace_back(std::async(std::launch::async, [node, this]()
                                                     { node->execute(&m_Graph); }));
    }

    void try_clear_futures()
    {
        for (auto &future : node_execute_futures)
            if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                future.get();

        node_execute_futures.erase(
            std::remove_if(node_execute_futures.begin(), node_execute_futures.end(),
                           [](std::future<void> &future)
                           {
                               if (future.valid())
                                   return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
                               return true;
                           }),
            node_execute_futures.end());
    }

    Graph m_Graph;
    ImTextureID m_HeaderBackground = nullptr;
    ImTextureID m_SaveIcon = nullptr;
    ImTextureID m_RestoreIcon = nullptr;
    ImTextureID m_PlayIcon = nullptr;
    const float m_TouchTime = 1.0f;
    std::map<ed::NodeId, float, NodeIdLess> m_NodeTouchTime;
    bool m_ShowOrdinals = false;
};

int main(int argc, char **argv)
{
    // 加入崩溃dump文件功能
    SetUnhandledExceptionFilter(ExceptionFilter);

    Example exampe("Blueprints", argc, argv);
    ifd::FileDialog::Instance().CreateTexture = [&exampe](uint8_t *data, int w, int h, char fmt) -> void *
    {
        return exampe.CreateTexture(data, w, h);
    };
    ifd::FileDialog::Instance().DeleteTexture = [&exampe](void *tex)
    {
        exampe.DestroyTexture(tex);
    };

    if (exampe.Create())
        return exampe.Run();

    return 0;
}