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
        if (in_port->has_value() == false)
        {
            // env->notifier->add_message("Image Source", "Image path is empty");
            return false;
        }
        std::string path;
        if (in_port->get(path) == false)
        {
            // env->notifier->add_message("Image Source", "Failed to get image path");
            return false;
        }
        try
        {
            auto image = cv::imread(path);
            if (image.empty())
            {
                // env->notifier->add_message("Image Source", "Failed to load image from " + path);
                return false;
            }
            out_port->set(image);
            return true;
        }
        catch (const std::exception &e)
        {
            // env->notifier->add_message("Image Source", "Failed to load image from " + path + ": " + e.what());
            return false;
        }
    };
    return node;
}

#endif // IMAGE_SOURCE_NODE_HPP
