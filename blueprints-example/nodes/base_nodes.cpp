#include "base_nodes.hpp"
#include "child_nodes/child_nodes.hpp"

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
            if (image.channels() == 1)
                cv::cvtColor(image, image, cv::COLOR_GRAY2RGBA);
            else if (image.channels() == 3)
                cv::cvtColor(image, image, cv::COLOR_RGB2RGBA);
        }
        catch (const std::exception &e)
        {
            this->Node->LastExecuteResult = ExecuteResult::ErrorPin(ID, e.what());
        }
        int width = image.cols;
        int height = image.rows;
        if (ImageTexture)
        {
            app->DestroyTexture(ImageTexture);
        }
        ImageTexture = app->CreateTexture(image.data, width, height);
    }
}

Pin &node_ui::get_virtual_input()
{
    if(virtual_input == nullptr)
        virtual_input = std::make_shared<Pin>(graph->get_next_id(), "Virtual Input", PinType::Object);
    return *virtual_input;
}

Pin &node_ui::get_virtual_output()
{
    if(virtual_output == nullptr)
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
    
    for(auto &[layer,layer_nodes] : layer_map)
    {
        auto layer_max_width = 0;
        for(auto &node : layer_nodes)
        {
            auto size = ed::GetNodeSize(node->ID);
            if(size.x > layer_max_width)
                layer_max_width = static_cast<int>(size.x);
        }
        printf("layer %d max width: %d\n", layer, layer_max_width);
        for(int i = 0; i < layer_nodes.size(); i++)
        {
            auto node = layer_nodes[i];
            ImVec2 pos = center;
            pos.x += (layer - 1) * (layer_max_width +20);
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

std::map<NodeType, NodeWorldGlobal::FactoryGroupFunc_t> NodeWorldGlobal::nodeFactories =
    {
        {NodeType::Blueprint, BlueprintNodes},
        {NodeType::BaseType, BaseTypeNodes},
        {NodeType::BaseConvert, BaseConvertNodes},
        {NodeType::BaseOperation, BaseOperationNodes},
        {NodeType::ImageFlow, {
                                  {"图像文件源", Spawn_ImageFileSource},
                                  {"图像查看器", Spawn_ImageViewer},
                                  {"获取图像大小", Spawn_ImageOperator_ImageGetSize},
                                  {"获取图像范围", Spawn_ImageOperator_ImageGetRect},
                                  {"获取图像通道数", Spawn_ImageOperator_ImageGetChannels},
                              }},
        {NodeType::ImageType, ImageTypeNodes},
        {NodeType::ImageValue, ImageMathNodes},
        {NodeType::ImageDraw, ImageDrawNodes},
        {NodeType::ImageSource, ImageSourceNodes},
        {NodeType::ImageOperation, {
                                       {"Mask Image", Spawn_ImageOperator_MaskImage},
                                       {"图像通道拆分", Spawn_ImageOperator_ImageChannelSplit},
                                       {"Image Channel Merge", Spawn_ImageOperator_ImageChannelMerge},
                                       {"Image And Mask Copy", Spawn_ImageOperator_ImageAndMaskCopy},
                                       {"Image ReSize", Spawn_ImageOperator_ImageReSize},
                                       {"Image Get Rect Image", Spawn_ImageOperator_ImageGetRectImage},
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