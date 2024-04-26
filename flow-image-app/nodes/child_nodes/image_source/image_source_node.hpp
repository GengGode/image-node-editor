#ifndef IMAGE_SOURCE_NODE_HPP
#define IMAGE_SOURCE_NODE_HPP

#include "base_node.hpp"
#include <filesystem>

std::shared_ptr<base_node> create_image_source_node(std::shared_ptr<global_env> &env)
{
    auto node = std::make_shared<base_node>();
    node->uuid = env->gen_next_node_id();
    node->name = "Image Source";
    node->label = std::to_string(env->node_next_id - 1) + " " + node->name;
    node->group_type = global_env::node_type_group::image_source;
    node->in_ports.emplace_back(std::make_shared<in_port>(
        env->gen_next_pin_id(),
        node, "Image Path", "图片路径", global_env::value_type::inout_string,
        in_port::source_type::self_define_or_other_node,
        std::string("resources/texture.png")));
    node->out_ports.emplace_back(std::make_shared<out_port>(
        env->gen_next_pin_id(),
        node, "Image", "图片", global_env::value_type::inout_image));
    node->event.on_execute = [node](std::shared_ptr<global_env> &env) -> bool
    {
        auto &in_port = node->in_ports[0];
        auto &out_port = node->out_ports[0];

        std::string path;
        if (in_port->get(path) == false)
        {
            node->last_error_opt = base_node::base_error{"Failed to get input value", base_node::base_error::error_source::node};
            return false;
        }

        try
        {
            auto image = cv::imread(path);
            if (image.empty())
            {
                node->last_error_opt = base_node::base_error{"Failed to load image from " + path, base_node::base_error::error_source::node};
                return false;
            }
            out_port->set(image);
        }
        catch (const std::exception &e)
        {
            node->last_error_opt = base_node::base_error{std::string("Failed to load image from ") + path + ": " + e.what(), base_node::base_error::error_source::node};
            return false;
        }
        return true;
    };
    return node;
}

#endif // IMAGE_SOURCE_NODE_HPP
