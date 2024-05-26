#pragma once
#include <imgui.h>
#include <imgui_internal.h>

#include "node_port_types.hpp"
#include "base_nodes.hpp"

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

        case PinType::EnumClass:
            return ImColor(51, 100, 215);
        case PinType::Enum:
            return ImColor(51, 200, 215);
        case PinType::Win32Handle:
            return ImColor(218, 200, 15);

        case PinType::Array:
            return ImColor(188, 188, 188);
        case PinType::ArrayElement:
            return ImColor(128, 128, 128);

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

        case PinType::EnumClass:
            iconType = IconType::Grid;
            break;
        case PinType::Enum:
            iconType = IconType::Circle;
            break;
        case PinType::Win32Handle:
            iconType = IconType::Circle;
            break;

        case PinType::Array:
            iconType = IconType::Grid;
            break;
        case PinType::ArrayElement:
            iconType = IconType::Circle;
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