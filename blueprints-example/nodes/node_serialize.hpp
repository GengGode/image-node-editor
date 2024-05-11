#pragma once

#include <string>
#include <vector>
#include <map>

#include <imgui.h>
#include <imgui_internal.h>
#include <opencv2/opencv.hpp>
#include <json.hpp>

#include "node_port_types.hpp"
#include "base_nodes.hpp"

struct NodeSerializer
{
    json::value operator()(const Node *node) const
    {
        json::object obj;
        obj["type"] = "node";
        obj["node_id"] = reinterpret_cast<int64>(node->ID.AsPointer());
        obj["node_name"] = node->Name;
        obj["node_type"] = static_cast<int>(node->Type);
        obj["node_type_name"] = nodeTypes.at(static_cast<int>(node->Type)).first;
        obj["node_color"] = json::serialize(node->Color, PortValueSerializer());
        obj["node_size"] = json::serialize(node->Size, PortValueSerializer());
        obj["node_position"] = json::serialize(node->Position, PortValueSerializer());
        json::array inputs;
        for (auto input : node->Inputs)
        {
            json::object input_obj;
            input_obj["type"] = "input";
            input_obj["input_id"] = reinterpret_cast<int64>(input.ID.AsPointer());
            input_obj["input_name"] = input.Name;
            input_obj["input_type"] = static_cast<int>(input.Type);
            input_obj["input_type_label"] = typeLabelNames.at(input.Type);
            input_obj["input_value"] = json::serialize(input.Value, PortValueSerializer());
            input_obj["input_kind"] = static_cast<int>(input.Kind);
            inputs.push_back(input_obj);
        }
        obj["inputs"] = inputs;
        json::array outputs;
        for (auto output : node->Outputs)
        {
            json::object output_obj;
            output_obj["type"] = "output";
            output_obj["output_id"] = reinterpret_cast<int64>(output.ID.AsPointer());
            output_obj["output_name"] = output.Name;
            output_obj["output_type"] = static_cast<int>(output.Type);
            output_obj["output_type_label"] = typeLabelNames.at(output.Type);
            output_obj["output_value"] = json::serialize(output.Value, PortValueSerializer());
            output_obj["output_kind"] = static_cast<int>(output.Kind);
            outputs.push_back(output_obj);
        }
        obj["outputs"] = outputs;
        return obj;
    }

    json::value operator()(const Link *link) const
    {
        json::object obj;
        obj["type"] = "link";
        obj["link_id"] = reinterpret_cast<int64>(link->ID.AsPointer());
        obj["link_start_pin_id"] = reinterpret_cast<int64>(link->StartPinID.AsPointer());
        obj["link_end_pin_id"] = reinterpret_cast<int64>(link->EndPinID.AsPointer());
        obj["link_color"] = json::serialize(link->Color, PortValueSerializer());
        return obj;
    }
};

struct NodeDeserializer
{
    bool operator()(const json::value &json, Node &node) const
    {
        if (json.is_object() && json.as_object().contains("type") && json.as_object().at("type").as_string() == "node")
        {
            int id = json.as_object().at("node_id").as_integer();
            node.ID = ed::NodeId(id);
            node.Name = json.as_object().at("node_name").as_string();
            node.Type = NodeType(json.as_object().at("node_type").as_integer());
            json::deserialize(json.as_object().at("node_color"), node.Color, PortValueDeserializer());
            json::deserialize(json.as_object().at("node_size"), node.Size, PortValueDeserializer());
            json::deserialize(json.as_object().at("node_position"), node.Position, PortValueDeserializer());
            auto inputs = json.as_object().at("inputs").as_array();
            for (auto input : inputs)
            {
                int input_id = input.as_object().at("input_id").as_integer();
                std::string input_name = input.as_object().at("input_name").as_string();
                PinType input_type = PinType(input.as_object().at("input_type").as_integer());
                port_value_t input_value;
                json::deserialize(input.as_object().at("input_value"), input_value, PortValueDeserializer());
                PinKind input_kind = PinKind(input.as_object().at("input_kind").as_integer());
                Pin pin(input_id, input_name.c_str(), input_type, input_value);
                pin.Kind = PinKind(input.as_object().at("input_kind").as_integer());
                pin.Node = &node;
                node.Inputs.push_back(pin);
            }
            auto outputs = json.as_object().at("outputs").as_array();
            for (auto output : outputs)
            {
                int output_id = output.as_object().at("output_id").as_integer();
                std::string output_name = output.as_object().at("output_name").as_string();
                PinType output_type = PinType(output.as_object().at("output_type").as_integer());
                port_value_t output_value;
                json::deserialize(output.as_object().at("output_value"), output_value, PortValueDeserializer());
                PinKind output_kind = PinKind(output.as_object().at("output_kind").as_integer());
                Pin pin(output_id, output_name.c_str(), output_type, output_value);
                pin.Kind = PinKind(output.as_object().at("output_kind").as_integer());
                pin.Node = &node;
                node.Outputs.push_back(pin);
            }
            return true;
        }
        return false;
    }

    bool operator()(const json::value &json, Link &link) const
    {
        if (json.is_object() && json.as_object().contains("type") && json.as_object().at("type").as_string() == "link")
        {
            int id = json.as_object().at("link_id").as_integer();
            link.ID = ed::LinkId(id);
            link.StartPinID = ed::PinId(json.as_object().at("link_start_pin_id").as_integer());
            link.EndPinID = ed::PinId(json.as_object().at("link_end_pin_id").as_integer());
            json::deserialize(json.as_object().at("link_color"), link.Color, PortValueDeserializer());
            return true;
        }
        return false;
    }
};
