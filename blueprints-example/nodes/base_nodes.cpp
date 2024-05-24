#include "base_nodes.hpp"

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include "../addons/imspinner.h"

#include "child_nodes/child_nodes.hpp"
#include "node_ui_colors.hpp"
#include "graph_ui.hpp"

void Pin::event_value_changed()
{
    if (std::this_thread::get_id() != NodeWorldGlobal::main_thread_id)
        return;
    if (Type == PinType::Image && app)
    {
        cv::Mat image = std::get<cv::Mat>(Value);
        if (image.empty())
            return;
        try
        {
            if ((image.depth() != CV_8U) || (image.depth() != CV_8S))
                // 根据单通道图像的深度(16,32,64)，将其转换为8位图像，除以255.0
                cv::normalize(image, image, 0, 255, cv::NORM_MINMAX, CV_8U);
            if (image.channels() == 1)
                cv::cvtColor(image, image, cv::COLOR_GRAY2RGBA);
            else if (image.channels() == 3)
                cv::cvtColor(image, image, cv::COLOR_RGB2RGBA);
            if (image.isContinuous() == false)
                image = image.clone();
            int width = image.cols;
            int height = image.rows;
            if (ImageTexture)
            {
                app->DestroyTexture(ImageTexture);
            }
            ImageTexture = app->CreateTexture(image.data, width, height);
        }
        catch (const std::exception &e)
        {
            this->Node->LastExecuteResult = ExecuteResult::ErrorPin(ID, e.what());
            return;
        }
    }
}

Pin &node_ui::get_virtual_input()
{
    if (virtual_input == nullptr)
        virtual_input = std::make_shared<Pin>(graph->get_next_id(), "Virtual Input", PinType::Object);
    return *virtual_input;
}

Pin &node_ui::get_virtual_output()
{
    if (virtual_output == nullptr)
        virtual_output = std::make_shared<Pin>(graph->get_next_id(), "Virtual Output", PinType::Object);
    return *virtual_output;
}

void Node::expand()
{
    ui.is_expanded = true;
    ui.virtual_input_links.clear();
    ui.virtual_output_links.clear();
}

void Node::collapse()
{
    ui.is_expanded = false;
    ui.virtual_input_links.clear();
    ui.virtual_output_links.clear();
    for (auto &input : Inputs)
    {
        auto exist_link = graph->FindPinLink(input.ID);
        if (exist_link == nullptr)
            continue;
        auto start_pin = graph->FindPin(exist_link->StartPinID);
        if (start_pin == nullptr)
            continue;
        if (start_pin->Node->ui.is_expanded == false)
            start_pin = &start_pin->Node->ui.get_virtual_output();
        ui.virtual_input_links.emplace_back(Link(graph->get_next_id(), start_pin->ID, ui.get_virtual_input().ID));
    }
    for (auto &output : Outputs)
    {
        auto exist_links = graph->FindPinLinks(output.ID);
        for (auto &exist_link : exist_links)
        {
            auto end_pin = graph->FindPin(exist_link->EndPinID);
            if (end_pin == nullptr)
                continue;
            if (end_pin->Node->ui.is_expanded == false)
                end_pin = &end_pin->Node->ui.get_virtual_input();
            ui.virtual_output_links.emplace_back(Link(graph->get_next_id(), ui.get_virtual_output().ID, end_pin->ID));
        }
    }
}

void Graph::auto_arrange()
{
    // 根据依赖关系排序

    // 需要计算位置的节点
    std::set<Node *> need_arrange_nodes;
    // 每个节点有一个依赖表，记录依赖的节点
    std::map<Node *, std::vector<Node *>> depend_map;
    // 每个节点都有一个关联表，记录依赖它的节点
    std::map<Node *, std::set<Node *>> relate_map;
    // 每层的节点
    std::map<int, std::vector<Node *>> layer_map;

    // 运行结束标志
    bool is_end = false;
    // 理论上运行的循环次数不会超过节点数
    size_t max_loop_limit = this->Nodes.size();
    // 运行循环次数
    int loop_count = 0;

    // 当前图的几何中心
    ImVec2 center = ImVec2(0, 0);
    for (auto &node : Nodes)
    {
        auto pos = ed::GetNodePosition(node.ID);
        center += pos;
    }
    center /= static_cast<float>(Nodes.size());
    printf("center: %f %f\n", center.x, center.y);

    // 需要运行所有节点
    for (auto &node : this->Nodes)
        need_arrange_nodes.insert(&node);

    // 生成依赖表
    {
        // 根据输入连接生成依赖表
        for (auto &node : this->Nodes)
        {
            for (auto &input : node.Inputs)
            {
                if (this->IsPinLinked(input.ID) == false)
                    continue;
                auto links = this->FindPinLinks(input.ID);
                for (auto &link : links)
                {
                    auto beginpin = this->FindPin(link->StartPinID);
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
        for (auto &node : this->Nodes)
        {
            for (auto &output : node.Outputs)
            {
                if (this->IsPinLinked(output.ID) == false)
                    continue;
                auto links = this->FindPinLinks(output.ID);
                for (auto &link : links)
                {
                    auto endpin = this->FindPin(link->EndPinID);
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
        // debug 打印依赖表
        for (auto &node : this->Nodes)
        {
            printf("node [%s] %d depend on: ", node.Name.c_str(), static_cast<int>(reinterpret_cast<int64>(node.ID.AsPointer())));
            for (auto &depend : depend_map[&node])
            {
                printf("%d ", static_cast<int>(reinterpret_cast<int64>(depend->ID.AsPointer())));
            }
            printf("\n");
        }
    }

    int current_layer = 0;
    while (need_arrange_nodes.size() > 0)
    {
        // 当前层的节点
        std::vector<Node *> layer_nodes;
        // 遍历还没有安排的节点
        for (auto &node : need_arrange_nodes)
        {
            // 如果没有依赖节点
            if (depend_map[node].size() == 0)
            {
                layer_nodes.push_back(node);
                continue;
            }

            // 如果依赖节点都已经安排
            // 计算交集，依赖的节点是否存在于需要安排的节点中
            std::set<Node *> intersect;
            std::set_intersection(depend_map[node].begin(), depend_map[node].end(), need_arrange_nodes.begin(), need_arrange_nodes.end(), std::inserter(intersect, intersect.begin()));

            // 如果有交集，说明依赖的节点还没有安排，否则说明依赖的节点都已经安排
            if (intersect.size() > 0)
                continue;

            layer_nodes.push_back(node);
        }

        for (auto &node : layer_nodes)
        {
            need_arrange_nodes.erase(node);
        }

        layer_map[current_layer] = layer_nodes;
        current_layer++;
    }

    for (auto &[layer, layer_nodes] : layer_map)
    {
        auto layer_max_width = 0;
        printf("===============\n");
        for (auto &node : layer_nodes)
        {
            auto size = ed::GetNodeSize(node->ID);
            if (static_cast<int>(size.x) > layer_max_width)
                layer_max_width = static_cast<int>(size.x);
            printf("node [%s] %d size: %f %f\n", node->Name.c_str(), static_cast<int>(reinterpret_cast<int64>(node->ID.AsPointer())), size.x, size.y);
        }
        printf("layer %d max width: %d\n", layer, layer_max_width);
        for (int i = 0; i < layer_nodes.size(); i++)
        {
            auto node = layer_nodes[i];
            ImVec2 pos = center;
            pos.x += (layer - 1) * (layer_max_width + 20);
            pos.y += (i - 1) * 300;
            ed::SetNodePosition(node->ID, pos);
            printf("node [%s] %d pos: %f %f\n", node->Name.c_str(), static_cast<int>(reinterpret_cast<int64>(node->ID.AsPointer())), pos.x, pos.y);
        }
    }

    // 获取排序后的几何中心
    ImVec2 new_center = ImVec2(0, 0);
    for (auto &node : Nodes)
    {
        auto pos = ed::GetNodePosition(node.ID);
        new_center += pos;
    }
    new_center /= static_cast<float>(Nodes.size());

    // 重新调整位置
    auto offset = center - new_center;
    for (auto &node : Nodes)
    {
        auto pos = ed::GetNodePosition(node.ID);
        pos += offset;
        ed::SetNodePosition(node.ID, pos);
    }

    printf("layer count: %d\n", static_cast<int>(layer_map.size()));
    for (auto &layer_nodes : layer_map)
    {
        printf("layer %d: ", layer_nodes.first);
        for (auto &node : layer_nodes.second)
        {
            printf("%d ", static_cast<int>(reinterpret_cast<int64>(node->ID.AsPointer())));
        }
        printf("\n");
    }
}

void node_ui::draw_input_pin(Pin &input)
{

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
    if (input.Type == PinType::Enum)
    {
        EnumValue enum_value;
        static int last_index = 0;
        input.GetValue(enum_value);
        last_index = enum_value.second;
        // 限制宽度
        ImGui::PushItemWidth(100.0f);
        auto &[enums, value] = enum_value;
        ImGui::Text("枚举值: %s", enums[value].c_str());

        ImGui::PopItemWidth();
        if (value != last_index)
        {
            input.SetValue(enum_value, [this]()
                           { this->graph->env.need_execute(); });
        }
        ImGui::Spring(0);
    }
}

void node_ui::draw_output_pin(Pin &output)
{

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
    if (output.Type == PinType::Enum)
    {
        EnumValue enum_value;
        static int last_index = 0;
        output.GetValue(enum_value);
        last_index = enum_value.second;
        // 限制宽度
        ImGui::PushItemWidth(100.0f);
        auto &[enums, value] = enum_value;
        // 直接绘制一个列表
        ImGui::BeginVertical("##combo_show");
        for (int i = 0; i < enums.size(); i++)
        {
            ImGui::BeginHorizontal(std::string("##combo_show_" + std::to_string(i)).c_str());
            // ImGui::BeginChild("##combo_show_child", ImVec2(100, 20));
            ImGui::LabelText("##combo_show_label", "%s", enums[i].c_str());
            bool is_selected = (value == i);
            ImGui::Checkbox("##combo_show_checkbox", &is_selected);
            if (is_selected)
                value = i;
            // ImGui::EndChild();
            ImGui::EndHorizontal();
        }
        ImGui::EndVertical();

        ImGui::PopItemWidth();
        if (value != last_index)
        {
            output.SetValue(enum_value, [this]()
                            { this->graph->env.need_execute(); });
        }
        ImGui::Spring(0);
    }

    if (output.Type == PinType::Image)
    {
        if (output.HasImage())
        {
            ImGui::Image((void *)(intptr_t)output.ImageTexture, ImVec2(100, 100));
            ImGui::Spring(0);
        }
    }
}

void node_ui::draw_node(Node *node)
{
    switch (node->Type)
    {
    case NodeType::Blueprint:
        // draw_blueprint_node(node);
        break;
    case NodeType::MaaTaskFlow:
        draw_image_node(node);
    case NodeType::BaseType:
        draw_image_node(node);
        break;
    case NodeType::BaseConvert:
        draw_image_node(node);
        break;
    case NodeType::BaseOperation:
        draw_image_node(node);
        break;
    case NodeType::ImageFlow:
        draw_image_node(node);
        break;
    case NodeType::ImageType:
        draw_image_node(node);
        break;
    case NodeType::ImageValue:
        draw_image_node(node);
        break;
    case NodeType::ImageDraw:
        draw_image_node(node);
        break;
    case NodeType::ImageSource:
        draw_image_node(node);
        break;
    case NodeType::ImageOperation:
        draw_image_node(node);
        break;
    case NodeType::ImageOperation_Threshold:
        draw_image_node(node);
        break;
    case NodeType::ImageOperation_Morphology:
        draw_image_node(node);
        break;
    case NodeType::ImageOperation_Filter:
        draw_image_node(node);
        break;
    case NodeType::ImageOperation_Edge:
        draw_image_node(node);
        break;
    case NodeType::ImageOperation_Feature:
        draw_image_node(node);
        break;
    case NodeType::ImageOther:
        draw_image_node(node);
        break;
    case NodeType::Simple:
        draw_simple_node(node);
        break;
    case NodeType::Comment:
        draw_comment_node(node);
        break;
    default:
        break;
    }
}
void node_ui::draw_comment_node(Node *node)
{
    const float commentAlpha = 0.75f;

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
    ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
    ed::BeginNode(node->ID);
    ImGui::PushID(node->ID.AsPointer());
    ImGui::BeginVertical("content");
    ImGui::BeginHorizontal("horizontal");
    ImGui::Spring(1);
    ImGui::TextUnformatted(node->Name.c_str());
    ImGui::Spring(1);
    ImGui::EndHorizontal();
    ed::Group(node->Size);
    ImGui::EndVertical();
    ImGui::PopID();
    ed::EndNode();
    ed::PopStyleColor(2);
    ImGui::PopStyleVar();

    if (ed::BeginGroupHint(node->ID))
    {
        // auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
        auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

        // ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

        auto min = ed::GetGroupMin();
        // auto max = ed::GetGroupMax();

        ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
        ImGui::BeginGroup();
        ImGui::TextUnformatted(node->Name.c_str());
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
void node_ui::draw_simple_node(Node *node)
{
    node->graph->ui.builder->Begin(node->ID);
    node->graph->ui.draw_node_input_pins(node);
    node->graph->ui.draw_node_output_pins(node);
    node->graph->ui.builder->End();
}
void node_ui::draw_image_node(Node *node)
{
    const auto isSimple = node->Type == NodeType::Simple;

    node->graph->ui.has_error = node->LastExecuteResult.has_error();
    node->graph->ui.has_error_and_hovered_on_port = false;
    node->graph->ui.has_error_and_hovered_on_node = false;

    if (node->graph->ui.has_error)
    {
        ImGui::PushStyleColor(ed::StyleColor_NodeBorder, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        node->graph->ui.context_error_opt = node->LastExecuteResult.Error;
    }

    bool hasOutputDelegates = false;
    for (auto &output : node->Outputs)
        if (output.Type == PinType::Delegate)
            hasOutputDelegates = true;

    node->graph->ui.builder->Begin(node->ID);
    if (!isSimple)
    {
        if (node->graph->ui.has_error)
        {
            if (node->LastExecuteResult.has_node_error())
                node->graph->ui.builder->Header(ImColor(255, 0, 0));
            else
                node->graph->ui.builder->Header(ImColor(200, 100, 100));
        }
        else
            node->graph->ui.builder->Header(node->Color);
        ImGui::Spring(0);
        ImGui::TextUnformatted(node->Name.c_str());
        if (node->graph->ui.has_error && ImGui::IsItemHovered())
        {
            auto error_source = node->graph->ui.context_error_opt.value().Source;
            if (std::holds_alternative<ed::NodeId>(error_source))
                node->graph->ui.has_error_and_hovered_on_node = true;
        }
        ImGui::Spring(1);
        ImGui::Dummy(ImVec2(0, 28));
        if (hasOutputDelegates)
        {
            ImGui::BeginVertical("delegates", ImVec2(0, 28));
            ImGui::Spring(1, 0);
            for (auto &output : node->Outputs)
            {
                if (output.Type != PinType::Delegate)
                    continue;

                auto alpha = ImGui::GetStyle().Alpha;
                if (node->graph->ui.new_link_pin && !CanCreateLink(node->graph->ui.new_link_pin, &output) && &output != node->graph->ui.new_link_pin)
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
        node->graph->ui.builder->EndHeader();
    }

    // ImGui::BeginGroup();
    if (node->ui.is_expanded)
    {
        node->graph->ui.draw_node_input_pins(node);
        node->graph->ui.draw_node_output_pins(node);
    }
    else
    {
        // 所有连线都汇总到一个输入和输出上
        {
            auto &virtual_input = node->ui.get_virtual_input();

            ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(255, 0, 0, 255));
            auto alpha = ImGui::GetStyle().Alpha;

            node->graph->ui.builder->Input(virtual_input.ID);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            ui::DrawPinIcon(virtual_input, graph->IsPinLinked(virtual_input.ID), (int)(alpha * 255));
            ImGui::PopStyleVar();

            node->graph->ui.builder->EndInput();
            ed::PopStyleColor();
        }

        {
            auto &virtual_output = node->ui.get_virtual_output();

            ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(255, 0, 0, 255));
            auto alpha = ImGui::GetStyle().Alpha;

            node->graph->ui.builder->Output(virtual_output.ID);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            ui::DrawPinIcon(virtual_output, graph->IsPinLinked(virtual_output.ID), (int)(alpha * 255));
            ImGui::PopStyleVar();

            node->graph->ui.builder->EndOutput();
            ed::PopStyleColor();
        }
    }
    // ImGui::EndGroup();

    //  footer
    if (!isSimple)
    {
        // builder->Footer();
        ImGui::Spring(1);
        std::string footer = "耗时：" + node->get_last_execute_time();
        ImGui::TextUnformatted(footer.c_str());
        if (node->is_running())
        {
            ImColor color = ui::get_color_from_thread_id(node->get_running_thread_id());
            // ImGui::Image(m_PlayIcon, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), color.Value);
            ImSpinner::SpinnerHerbertBalls3D("wiat", 16, 3.f, color, 2.0f);
        }
        // builder->EndFooter();
    }

    node->graph->ui.builder->End();

    if (node->graph->ui.has_error)
    {
        ImGui::PopStyleColor();
        ed::Suspend();
        // 绘制错误提示
        if (node->graph->ui.has_error_and_hovered_on_node || node->graph->ui.has_error_and_hovered_on_port)
        {
            // ImGui::SetTooltip("错误: %s", context_error_opt.value().Message.c_str());
            auto error_message = node->graph->ui.context_error_opt.value().Message;
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
void node_ui::draw_flow_node(Node *node)
{
}
std::map<NodeType, NodeWorldGlobal::FactoryGroupFunc_t> NodeWorldGlobal::nodeFactories =
    {
        {NodeType::Blueprint, BlueprintNodes},
        {NodeType::MaaTaskFlow, MaaTaskFlowNodes},
        {NodeType::BaseType, BaseTypeNodes},
        {NodeType::BaseConvert, BaseConvertNodes},
        {NodeType::BaseOperation, BaseOperationNodes},
        {NodeType::ImageFlow, {
                                  {"图像查看器", Spawn_ImageViewer},
                                  {"写入本地文件", Spawn_ImageWriteLocalFile},
                                  {"写入Raw文件", Spawn_ImageWriteRawFile},
                                  {"获取图像大小", Spawn_ImageOperator_ImageGetSize},
                                  {"获取图像范围", Spawn_ImageOperator_ImageGetRect},
                                  {"获取图像通道数", Spawn_ImageOperator_ImageGetChannels},
                                  {"获取图像深度", Spawn_ImageOperator_ImageGetDepth},
                                  {"获取图像信息", Spawn_ImageOperator_ImageGetAllInfo},
                              }},
        {NodeType::ImageType, ImageTypeNodes},
        {NodeType::ImageValue, ImageMathNodes},
        {NodeType::ImageDraw, ImageDrawNodes},
        {NodeType::ImageSource, ImageSourceNodes},
        {NodeType::ImageOperation, {
                                       {"OCR 文本", Spawn_ImageOperator_OcrText},
                                       {"Mask Image", Spawn_ImageOperator_MaskImage},
                                       {"图像通道拆分", Spawn_ImageOperator_ImageChannelSplit},
                                       {"图像通道合并", Spawn_ImageOperator_ImageChannelMerge},
                                       {"获取遮罩图像", Spawn_ImageOperator_ImageAndMaskCopy},
                                       {"调整图像大小", Spawn_ImageOperator_ImageReSize},
                                       {"获取范围图像", Spawn_ImageOperator_ImageGetRectImage},
                                       {"范围图像覆盖图像", Spawn_ImageOperator_RectImageToImage},
                                       {"水平拼接图像", Spawn_ImageOperator_HConcatenateImages},
                                       {"垂直拼接图像", Spawn_ImageOperator_VConcatenateImages},
                                       {"网格拆分图像", Spawn_ImageOperator_GridSplitImages},
                                   }},
        {NodeType::ImageOperation_Threshold, ImageOperatorThresholdNodes},
        {NodeType::ImageOperation_Morphology, ImageOperatorMorphologyNodes},
        {NodeType::ImageOperation_Filter, ImageOperatorFilterNodes},
        {NodeType::ImageOperation_Edge, ImageOperatorEdgeNodes},
        {NodeType::ImageOperation_Feature, ImageFeatureNodes},
        {NodeType::ImageOther, ImageColorConvertNodes},
        {NodeType::Simple, {
                               {"Int to String", SpawnIntToStringNode},
                               {"Message", SpawnMessageNode},
                               {"Less", SpawnLessNode},
                               {"Weird", SpawnWeirdNode},
                           }},
        {NodeType::Comment, {
                                {"Comment", SpawnComment},
                            }},
};

std::map<std::pair<PinType, PinType>, NodeWorldGlobal::NodeFactory_t> NodeWorldGlobal::registerLinkAutoConvertNodeFactories = {
    {{PinType::Bool, PinType::Int}, SpawnBoolToIntNode},
    {{PinType::Bool, PinType::Float}, SpawnBoolToFloatNode},
    {{PinType::Bool, PinType::String}, SpawnBoolToStringNode},
    {{PinType::Int, PinType::Bool}, SpawnIntToBoolNode},
    {{PinType::Int, PinType::Float}, SpawnIntToFloatNode},
    {{PinType::Int, PinType::String}, SpawnIntToStringNode},
    {{PinType::Float, PinType::Bool}, SpawnFloatToBoolNode},
    {{PinType::Float, PinType::Int}, SpawnFloatToIntNode},
    {{PinType::Float, PinType::String}, SpawnFloatToStringNode},
    {{PinType::String, PinType::Bool}, SpawnStringToBoolNode},
    {{PinType::String, PinType::Int}, SpawnStringToIntNode},
    {{PinType::String, PinType::Float}, SpawnStringToFloatNode},
    {{PinType::Size, PinType::Point}, Spawn_ImageOperator_SizeToPoint},
    {{PinType::Point, PinType::Size}, Spawn_ImageOperator_PointToSize},
};