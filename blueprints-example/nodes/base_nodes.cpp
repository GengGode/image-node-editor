#include "base_nodes.hpp"
#include "child_nodes/child_nodes.hpp"

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