#include "base_node.hpp"
#include "child_nodes/child_nodes.hpp"

std::map<global_env::node_type_group, std::vector<std::pair<std::string, std::function<std::shared_ptr<base_node>(std::shared_ptr<global_env> &)>>>> global_env::node_builder_factories = {
    {global_env::node_type_group::image_source,
     {
         {"Image Source", create_image_source_node},
         {"Image Source 2", create_image_source_node},
     }},
    {global_env::node_type_group::arithmetic,
     {{"empty", create_image_source_node}}},
    {global_env::node_type_group::io,
     {{"empty", create_image_source_node}}},
    {global_env::node_type_group::other,
     {{"empty", create_image_source_node}}},
    {global_env::node_type_group::convert,
     {{"empty", create_image_source_node}}},
    {global_env::node_type_group::compare,
     {{"empty", create_image_source_node}}}};