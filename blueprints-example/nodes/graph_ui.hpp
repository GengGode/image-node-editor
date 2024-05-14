#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "base_nodes.hpp"

inline void GraphUi::draw_links()
{
    for (auto &link : graph->Links)
        ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);
}

inline void GraphUi::draw_virtual_links()
{

    for (auto &node : graph->Nodes)
    {
        if (node.ui.is_expanded == false)
        {
            for (auto &link : node.ui.virtual_input_links)
            {
                ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);
            }

            for (auto &link : node.ui.virtual_output_links)
            {
                ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);
            }
        }
    }
}

inline void GraphUi::draw_node_input_pins(Node *node)
{
    for (auto &input : node->Inputs)
    {
        ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(255, 0, 0, 255));
        auto alpha = ImGui::GetStyle().Alpha;
        if (new_link_pin && !CanCreateLink(new_link_pin, &input) && &input != new_link_pin)
            alpha = alpha * (48.0f / 255.0f);

        builder->Input(input.ID);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        ui::DrawPinIcon(input, graph->IsPinLinked(input.ID), (int)(alpha * 255));
        ImGui::Spring(0);
        if (!input.Name.empty())
        {
            ImGui::TextUnformatted(input.Name.c_str());
            ImGui::Spring(0);
        }
        if (node->Name == "图像查看器")
        {
            if (input.HasImage())
            {
                if (ImGui::Button("show output"))
                {
                    std::string name = "output " + std::to_string((int)(size_t)node->ID);
                    auto window = ImGui::FindWindowByName(name.c_str());
                    ImGui::FocusWindow(window);
                }
            }
        }
        if (input.Type == PinType::Bool)
        {
            bool value = false;
            static bool last_value = false;
            input.GetValue(value);
            last_value = value;
            ImGui::Checkbox("##edit", &value);
            if (value != last_value)
            {
                input.SetValue(value, [this]()
                               { this->graph->env.need_execute(); });
            }
            ImGui::Spring(0);
        }
        if (input.Type == PinType::Int)
        {
            int value = 0;
            static int last_value = 0;
            input.GetValue(value);
            last_value = value;
            // 限制宽度
            ImGui::PushItemWidth(100.0f);
            ImGui::DragInt("##edit", &value);
            ImGui::PopItemWidth();
            if (value != last_value)
            {
                input.SetValue(value, [this]()
                               { this->graph->env.need_execute(); });
            }
            ImGui::Spring(0);
        }
        if (input.Type == PinType::Float)
        {
            float value = 0.0f;
            static float last_value = 0.0f;
            input.GetValue(value);
            last_value = value;
            // 限制宽度
            ImGui::PushItemWidth(100.0f);
            ImGui::DragFloat("##edit", &value, 0.5f);
            ImGui::PopItemWidth();
            if (value != last_value)
            {
                input.SetValue(value, [this]()
                               { this->graph->env.need_execute(); });
            }
            ImGui::Spring(0);
        }
        if (input.Type == PinType::String)
        {
            std::string inputStr;
            bool res = input.GetValue(inputStr);
            if (!res)
                printf("Error: %s\n", inputStr.c_str());
            char buffer[128] = {0};
            std::copy(inputStr.begin(), inputStr.end(), buffer);

            ImGui::PushItemWidth(100.0f);
            ImGui::InputText("##edit", buffer, 127);
            ImGui::PopItemWidth();
            if (buffer != inputStr)
            {
                input.SetValue(std::string(buffer), [this]()
                               { this->graph->env.need_execute(); });
            }

            ImGui::Spring(0);
        }

        ImGui::PopStyleVar();
        builder->EndInput();
        if (has_error && ImGui::IsItemHovered())
        {
            auto error_source = context_error_opt.value().Source;
            if (std::holds_alternative<ed::PinId>(error_source))
                has_error_and_hovered_on_port = true;
        }
        ed::PopStyleColor();
    }
}
inline void GraphUi::draw_node_output_pins(Node *node)
{
    for (auto &output : node->Outputs)
    {
        if (!node->is_simple_node() && output.Type == PinType::Delegate)
            continue;

        auto alpha = ImGui::GetStyle().Alpha;
        if (new_link_pin && !CanCreateLink(new_link_pin, &output) && &output != new_link_pin)
            alpha = alpha * (48.0f / 255.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        builder->Output(output.ID);

        if (output.Type == PinType::String)
        {
            std::string outputStr;
            bool res = output.GetValue(outputStr);
            if (res)
            {
                char buffer[128] = {0};
                std::copy(outputStr.begin(), outputStr.end(), buffer);

                ImGui::PushItemWidth(100.0f);
                ImGui::InputText("##edit", buffer, 127);
                ImGui::PopItemWidth();
                if (buffer != outputStr)
                {
                    output.SetValue(std::string(buffer), [this]()
                                    { this->graph->env.need_execute(); });
                }
                ImGui::Spring(0);
            }
        }
        if (output.Type == PinType::KeyPoints)
        {
            KeyPoints keyPoints;
            bool res = output.GetValue(keyPoints);
            if (res)
            {
                auto size = keyPoints.size();

                ImGui::PushItemWidth(100.0f);
                ImGui::Text("数量: %d", size);
                ImGui::PopItemWidth();

                ImGui::Spring(0);
            }
        }
        if (output.Type == PinType::Feature)
        {
            Feature feature;
            bool res = output.GetValue(feature);
            if (res)
            {
                auto size = feature.first.size();

                ImGui::PushItemWidth(100.0f);
                ImGui::Text("数量: %d", size);
                ImGui::PopItemWidth();

                ImGui::Spring(0);
            }
        }
        if (output.Type == PinType::Matches)
        {
            std::vector<cv::DMatch> matches;
            bool res = output.GetValue(matches);
            if (res)
            {
                auto size = matches.size();

                ImGui::PushItemWidth(100.0f);
                ImGui::Text("数量: %d", size);
                ImGui::PopItemWidth();

                ImGui::Spring(0);
            }
        }
        if (output.Type == PinType::Circles)
        {
            Circles circles;
            bool res = output.GetValue(circles);
            if (res)
            {
                auto size = circles.size();

                ImGui::PushItemWidth(100.0f);
                ImGui::Text("数量: %d", size);
                ImGui::PopItemWidth();

                ImGui::Spring(0);
            }
        }

        if (output.Type == PinType::Image)
        {
            if (output.HasImage())
            {
                ImGui::Image((void *)(intptr_t)output.ImageTexture, ImVec2(100, 100));
                ImGui::Spring(0);
            }
        }
        if (!output.Name.empty())
        {
            ImGui::Spring(0);
            ImGui::TextUnformatted(output.Name.c_str());
        }
        ImGui::Spring(0);
        ui::DrawPinIcon(output, graph->IsPinLinked(output.ID), (int)(alpha * 255));
        ImGui::PopStyleVar();
        builder->EndOutput();
    }
}
inline void GraphUi::draw_image_nodes()
{
    // 图片节点 绘制
    for (auto &node : graph->Nodes)
    {
        if (node.Type == NodeType::Comment)
            continue;

        const auto isSimple = node.Type == NodeType::Simple;

        has_error = node.LastExecuteResult.has_error();
        has_error_and_hovered_on_port = false;
        has_error_and_hovered_on_node = false;

        if (has_error)
        {
            ImGui::PushStyleColor(ed::StyleColor_NodeBorder, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            context_error_opt = node.LastExecuteResult.Error;
        }

        bool hasOutputDelegates = false;
        for (auto &output : node.Outputs)
            if (output.Type == PinType::Delegate)
                hasOutputDelegates = true;

        builder->Begin(node.ID);
        if (!isSimple)
        {
            if (has_error)
            {
                if (node.LastExecuteResult.has_node_error())
                    builder->Header(ImColor(255, 0, 0));
                else
                    builder->Header(ImColor(200, 100, 100));
            }
            else
                builder->Header(node.Color);
            ImGui::Spring(0);
            ImGui::TextUnformatted(node.Name.c_str());
            if (has_error && ImGui::IsItemHovered())
            {
                auto error_source = context_error_opt.value().Source;
                if (std::holds_alternative<ed::NodeId>(error_source))
                    has_error_and_hovered_on_node = true;
            }
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(0, 28));
            if (hasOutputDelegates)
            {
                ImGui::BeginVertical("delegates", ImVec2(0, 28));
                ImGui::Spring(1, 0);
                for (auto &output : node.Outputs)
                {
                    if (output.Type != PinType::Delegate)
                        continue;

                    auto alpha = ImGui::GetStyle().Alpha;
                    if (new_link_pin && !CanCreateLink(new_link_pin, &output) && &output != new_link_pin)
                        alpha = alpha * (48.0f / 255.0f);

                    ed::BeginPin(output.ID, ed::PinKind::Output);
                    ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
                    ed::PinPivotSize(ImVec2(0, 0));
                    ImGui::BeginHorizontal(output.ID.AsPointer());
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                    if (!output.Name.empty())
                    {
                        ImGui::TextUnformatted(output.Name.c_str());
                        ImGui::Spring(0);
                    }
                    ui::DrawPinIcon(output, graph->IsPinLinked(output.ID), (int)(alpha * 255));
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
            builder->EndHeader();
        }

        if (node.ui.is_expanded)
        {
            draw_node_input_pins(&node);
            draw_node_output_pins(&node);
        }
        else
        {
            // 所有连线都汇总到一个输入和输出上
            {
                auto &virtual_input = node.ui.get_virtual_input();

                ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(255, 0, 0, 255));
                auto alpha = ImGui::GetStyle().Alpha;

                builder->Input(virtual_input.ID);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                ui::DrawPinIcon(virtual_input, graph->IsPinLinked(virtual_input.ID), (int)(alpha * 255));
                ImGui::PopStyleVar();

                builder->EndInput();
                ed::PopStyleColor();
            }

            {
                auto &virtual_output = node.ui.get_virtual_output();

                ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(255, 0, 0, 255));
                auto alpha = ImGui::GetStyle().Alpha;

                builder->Output(virtual_output.ID);

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                ui::DrawPinIcon(virtual_output, graph->IsPinLinked(virtual_output.ID), (int)(alpha * 255));
                ImGui::PopStyleVar();

                builder->EndOutput();
                ed::PopStyleColor();
            }
        }

        // footer
        if (!isSimple)
        {
            ImGui::Spring(1);
            std::string footer = "耗时：" + node.get_last_execute_time();
            ImGui::TextUnformatted(footer.c_str());
            if (node.is_running())
            {
                ImColor color = ui::get_color_from_thread_id(node.get_running_thread_id());
                //ImGui::Image(m_PlayIcon, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), color.Value);
                ImSpinner::SpinnerHerbertBalls3D("wiat", 16, 3.f, color, 2.0f);
            }
            //builder->EndFooter();
        }

        builder->End();

        if (has_error)
        {
            ImGui::PopStyleColor();
            ed::Suspend();
            if (has_error_and_hovered_on_node || has_error_and_hovered_on_port)
            {
                // ImGui::SetTooltip("错误: %s", context_error_opt.value().Message.c_str());
                auto error_message = context_error_opt.value().Message;
                if (error_message.size() < 64)
                {
                    ImGui::SetTooltip("错误: \n%s", error_message.c_str());
                }
                else
                {
                    ImGui::BeginTooltip();
                    ImGui::BeginChild("error", ImVec2(240, 300));
                    ImGui::TextWrapped("错误: \n%s", error_message.c_str());
                    ImGui::SetScrollHereY(1.0f);
                    ImGui::EndChild();
                    ImGui::EndTooltip();
                }
            }
            ed::Resume();
        }
    }
}

inline void GraphUi::draw_comment_nodes()
{
    // Comment nodes
    for (auto &node : graph->Nodes)
    {
        if (node.Type != NodeType::Comment)
            continue;

        const float commentAlpha = 0.75f;

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
        ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
        ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
        ed::BeginNode(node.ID);
        ImGui::PushID(node.ID.AsPointer());
        ImGui::BeginVertical("content");
        ImGui::BeginHorizontal("horizontal");
        ImGui::Spring(1);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1);
        ImGui::EndHorizontal();
        ed::Group(node.Size);
        ImGui::EndVertical();
        ImGui::PopID();
        ed::EndNode();
        ed::PopStyleColor(2);
        ImGui::PopStyleVar();

        if (ed::BeginGroupHint(node.ID))
        {
            // auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
            auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

            // ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

            auto min = ed::GetGroupMin();
            // auto max = ed::GetGroupMax();

            // FIX: error C2676: 二进制“-”:“ImVec2”不定义该运算符或到预定义运算符可接收的类型的转换
            // ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
            ImGui::SetCursorScreenPos(ImVec2(min.x + 8, min.y - (ImGui::GetTextLineHeightWithSpacing() + 4)));
            ImGui::BeginGroup();
            ImGui::TextUnformatted(node.Name.c_str());
            ImGui::EndGroup();

            auto drawList = ed::GetHintBackgroundDrawList();

            auto hintBounds = ImGui_GetItemRect();
            auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

            drawList->AddRectFilled(
                hintFrameBounds.GetTL(),
                hintFrameBounds.GetBR(),
                IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

            drawList->AddRect(
                hintFrameBounds.GetTL(),
                hintFrameBounds.GetBR(),
                IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

            // ImGui::PopStyleVar();
        }
        ed::EndGroupHint();
    }
}