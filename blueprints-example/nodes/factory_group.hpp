#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <algorithm>
// 树状右键菜单
template <class F>
class factory_group
{
public:
    using functor = F;
    using group_ptr = std::shared_ptr<factory_group<functor>>;
    using name_group = std::pair<std::string, group_ptr>;
    using groups = std::vector<std::pair<std::string, std::shared_ptr<factory_group<functor>>>>;
    using groups_ptr = std::shared_ptr<groups>;

public:
    std::optional<functor> factory_opt = std::nullopt;
    groups_ptr sub_groups = std::make_shared<groups>();

public:
    factory_group() = default;
    factory_group(std::optional<functor> factory_opt, groups_ptr sub_groups) : factory_opt(factory_opt), sub_groups(sub_groups) {}

public:
    bool is_sub_group() { return !sub_groups->empty(); }
    bool is_has_factory() { return factory_opt.has_value(); }
    bool is_empty() { return sub_groups->empty() && !factory_opt.has_value(); }
    void factory_to_sub_group(std::string name)
    {
        sub_groups->push_back(std::make_pair(name, std::make_shared<factory_group<functor>>(factory_opt, std::make_shared<groups>())));
    }
};

template <class F>
class factory_group_manager
{
    factory_group_manager() = default;

public:
    ~factory_group_manager() = default;

public:
    using functor = F;
    using factory_groups = std::vector<std::pair<std::string, std::shared_ptr<factory_group<functor>>>>;
    std::unordered_map<std::string, std::shared_ptr<factory_groups>> factory_root_groups;

public:
    static factory_group_manager &get_instance()
    {
        static factory_group_manager instance;
        return instance;
    }

    std::shared_ptr<factory_groups> get_root_groups(std::string name)
    {
        if (factory_root_groups.find(name) == factory_root_groups.end())
            factory_root_groups[name] = std::make_shared<factory_groups>();
        return factory_root_groups[name];
    }

private:
    bool register_group_to_groups(std::string path, std::vector<std::string> paths, const std::string &name, std::shared_ptr<factory_groups> groups, functor factory)
    {
        auto groups_it = groups->begin();

        if (path != "")
        {
            groups_it = std::find_if(groups->begin(), groups->end(), [path](auto &group)
                                     { return group.first == path; });
            if (groups_it == groups->end())
            {
                groups->push_back(std::make_pair(path, std::make_shared<factory_group<functor>>()));
                groups_it = groups->end() - 1;
            }
            groups = groups_it->second->sub_groups;
        }
        if (paths.size() == 0)
        {
            // 在该组进行插入，
            // 1.如果是新建组，就设置为factory_opt，
            // 2.如果已经有factory_opt但是不是sub_group就转化为sub_group再插入sub_gorup
            // 3.如果是sub_group就插入
            if (groups_it->second->is_empty())
            {
                groups_it->second->factory_opt = factory;
                return true;
            }

            if (groups_it->second->is_has_factory())
                groups_it->second->factory_to_sub_group(groups_it->first);
            groups_it->second->sub_groups->push_back(std::make_pair(name, std::make_shared<factory_group<functor>>()));
            groups_it->second->sub_groups->back().second->factory_opt = factory;

            return true;
        }

        if (!groups_it->second->is_sub_group() && groups_it->second->is_has_factory())
        {
            groups_it->second->factory_to_sub_group(groups_it->first);
        }

        auto next_path = paths[0];
        paths.erase(paths.begin());
        return register_group_to_groups(next_path, paths, name, groups, factory);
    }

    bool register_groups_to_groups(std::string path, std::vector<std::string> paths, std::shared_ptr<factory_groups> groups, std::shared_ptr<factory_groups> sub_groups)
    {
        auto groups_it = groups->begin();

        if (path != "")
        {
            groups_it = std::find_if(groups->begin(), groups->end(), [path](auto &group)
                                     { return group.first == path; });
            if (groups_it == groups->end())
            {
                groups->push_back(std::make_pair(path, std::make_shared<factory_group<functor>>()));
                groups_it = groups->end() - 1;
            }
            groups = groups_it->second->sub_groups;
        }
        if (paths.size() == 0)
        {
            // 在该组进行插入，
            // 1.如果是新建组，就设置为factory_opt，
            // 2.如果已经有factory_opt但是不是sub_group就转化为sub_group再插入sub_gorup
            // 3.如果是sub_group就插入
            if (groups_it->second->is_empty())
            {
                groups_it->second->sub_groups = sub_groups;
                return true;
            }

            if (groups_it->second->is_has_factory())
                groups_it->second->factory_to_sub_group(groups_it->first);
            groups_it->second->sub_groups->push_back(std::make_pair(sub_groups->begin()->first, std::make_shared<factory_group<functor>>()));
            groups_it->second->sub_groups->back().second->sub_groups = sub_groups;

            return true;
        }

        if (not groups_it->second->is_sub_group() && groups_it->second->is_has_factory())
        {
            groups_it->second->factory_to_sub_group(groups_it->first);
        }

        auto next_path = paths[0];
        paths.erase(paths.begin());
        return register_node_groups_to_groups(next_path, paths, groups, sub_groups);
    }

public:
    void register_group(std::shared_ptr<factory_groups> root_groups, const std::vector<std::string> &paths, functor factory)
    {
        std::shared_ptr<factory_groups> groups = root_groups;
        auto paths_copy = paths;
        auto path = paths_copy[0];
        auto name = paths.back();
        paths_copy.erase(paths_copy.begin());
        register_group_to_groups(path, paths_copy, name, groups, factory);
    }
    void register_group(std::shared_ptr<factory_groups> root_groups, const std::vector<std::string> &paths, std::shared_ptr<factory_groups> sub_groups)
    {
        std::shared_ptr<factory_groups> groups = root_groups;
        auto paths_copy = paths;
        auto path = paths_copy[0];
        paths_copy.erase(paths_copy.begin());
        register_groups_to_groups(path, paths_copy, groups, sub_groups);
    }
    void register_group_from_absolute_path(std::shared_ptr<factory_groups> root_groups, const std::string absolute_path, functor factory)
    {
        std::vector<std::string> paths;
        std::string path;
        for (auto &c : absolute_path)
        {
            if (c == '/')
            {
                if (path != "")
                    paths.push_back(path);
                path = "";
            }
            else
            {
                path += c;
            }
        }
        if (path != "")
            paths.push_back(path);
        register_group(root_groups, paths, factory);
    }
    void register_group_from_factorys(std::shared_ptr<factory_groups> root_groups, std::vector<std::pair<std::string, functor>> factorys)
    {
        for (auto &[path, factory] : factorys)
            register_group_from_absolute_path(root_groups, path, factory);
    }

    enum class stack_status
    {
        begin,
        content,
        end
    };

    void for_each_group(std::shared_ptr<factory_groups> root_groups, std::function<bool(std::vector<std::string>, stack_status, bool, std::shared_ptr<factory_group<functor>>)> pred, std::vector<std::string> stack = {}, bool last_menu_status = true)
    {
        if (!root_groups)
            return;
        for (auto &[path, groups] : *root_groups)
        {
            if (groups->sub_groups->size() == 0)
            {
                stack.push_back(path);
                pred(stack, stack_status::content, last_menu_status, groups);
                stack.pop_back();
                continue;
            }
            stack.push_back(path);
            auto menu_status = pred(stack, stack_status::begin, last_menu_status, groups);
            for_each_group(groups->sub_groups, pred, stack, menu_status);
            pred(stack, stack_status::end, menu_status, groups);
            stack.pop_back();
        }
    }
};
// std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)>;
struct Node;
struct Application;
using factory_func_t = std::function<Node *(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)>;
using node_factorys = factory_group_manager<factory_func_t>;
//class value;
//using value_factorys = factory_group_manager<std::function<std::shared_ptr<value>()>>;

