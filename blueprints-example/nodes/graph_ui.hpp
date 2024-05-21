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

        node->ui.draw_input_pin(input);

        ImGui::PopStyleVar();
        builder->EndInput();
        ed::PopStyleColor();

        if (has_error && ImGui::IsItemHovered())
        {
            auto &error_source = context_error_opt.value().Source;
            if (std::holds_alternative<ed::PinId>(error_source))
                has_error_and_hovered_on_port = true;
        }
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

        node->ui.draw_output_pin(output);

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
inline void GraphUi::draw_nodes()
{
    for (auto &node : graph->Nodes)
    {
        node.ui.draw_node(&node);
    }
}
