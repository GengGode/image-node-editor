#ifndef BASE_NODE_HPP
#define BASE_NODE_HPP

#include "../utilities/builders.h"
#include "../utilities/widgets.h"
#include "../notifiers/Notifier.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <application.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <optional>
#include <future>

#include <opencv2/opencv.hpp>

namespace ed = ax::NodeEditor;

struct base_node;

struct global_env : std::enable_shared_from_this<global_env>
{
    using port_value_t = std::variant<int, float, bool, std::string, cv::Mat>;

    template <typename T>
    static bool is_equal(const T &lft, const T &rht)
    {
        return lft == rht;
    }

    template <>
    static bool is_equal(const cv::Mat &lft, const cv::Mat &rht)
    {
        if (lft.dims == rht.dims &&
            lft.size == rht.size &&
            lft.elemSize() == rht.elemSize())
        {
            if (lft.isContinuous() && rht.isContinuous())
            {
                return 0 == memcmp(lft.ptr(), rht.ptr(), lft.total() * lft.elemSize());
            }
            else
            {
                const cv::Mat *arrays[] = {&lft, &rht, 0};
                uchar *ptrs[2];
                cv::NAryMatIterator it(arrays, ptrs, 2);
                for (unsigned int p = 0; p < it.nplanes; p++, ++it)
                    if (0 != memcmp(it.ptrs[0], it.ptrs[1], it.size * lft.elemSize()))
                        return false;

                return true;
            }
        }

        return false;
    }

    static bool is_equal(const port_value_t &lft, const port_value_t &rht)
    {
        return std::visit([&](auto &&arg1, auto &&arg2)
                          { if (typeid(arg1)!= typeid(arg2))
                                return false;
                            return is_equal(arg1, arg2); },
                          lft, rht);
    }

    enum class value_type
    {
        inout_int,
        inout_float,
        inout_bool,
        inout_string,
        inout_image,

        inout_delegate
    };
    inline static const std::map<size_t, value_type> type_map = {
        {typeid(int).hash_code(), value_type::inout_int},
        {typeid(float).hash_code(), value_type::inout_float},
        {typeid(bool).hash_code(), value_type::inout_bool},
        {typeid(std::string).hash_code(), value_type::inout_string},
        {typeid(cv::Mat).hash_code(), value_type::inout_image},
    };
    std::string enum_to_string(value_type type)
    {
        switch (type)
        {
        case value_type::inout_int:
            return "int";
        case value_type::inout_float:
            return "float";
        case value_type::inout_bool:
            return "bool";
        case value_type::inout_string:
            return "string";
        case value_type::inout_image:
            return "image";
        default:
            return "unknown";
        }
    }
    enum class node_type_group
    {
        image_source,
        arithmetic,
        logic,
        compare,
        convert,
        io,
        other
    };
    inline static const std::map<node_type_group, std::string> node_type_group_map = {
        {node_type_group::image_source, "image_source"},
        {node_type_group::arithmetic, "arithmetic"},
        {node_type_group::logic, "logic"},
        {node_type_group::compare, "compare"},
        {node_type_group::convert, "convert"},
        {node_type_group::io, "io"},
        {node_type_group::other, "other"}};
    std::string enum_to_string(node_type_group group)
    {
        switch (group)
        {
        case node_type_group::image_source:
            return "image_source";
        case node_type_group::arithmetic:
            return "arithmetic";
        case node_type_group::logic:
            return "logic";
        case node_type_group::compare:
            return "compare";
        case node_type_group::convert:
            return "convert";
        case node_type_group::io:
            return "io";
        case node_type_group::other:
            return "other";
        default:
            return "unknown";
        }
    }
    static std::map<node_type_group, std::vector<std::pair<std::string, std::function<std::shared_ptr<base_node>(std::shared_ptr<global_env> &)>>>> node_builder_factories;

    struct ui
    {
        std::function<void(std::shared_ptr<global_env> &env)> on_draw;
    };
    struct link_ui : ui
    {
        ImColor color;
    };

    struct port_ui : ui
    {
        ImColor color;
    };

    struct base_node_ui : ui
    {
        ImColor color;
        std::optional<bool> last_execute_result_opt;
    };

    struct event
    {
        std::function<void(std::shared_ptr<global_env> &env)> on_click;
        std::function<void(std::shared_ptr<global_env> &env)> on_hover;

        std::function<void(std::shared_ptr<global_env> &env)> on_drag;
        std::function<void(std::shared_ptr<global_env> &env)> on_drop;

        std::function<void(std::shared_ptr<global_env> &env)> on_connect;
        std::function<void(std::shared_ptr<global_env> &env)> on_disconnect;

        std::function<void(std::shared_ptr<global_env> &env)> on_create;
        std::function<void(std::shared_ptr<global_env> &env)> on_destroy;

        std::function<void(std::shared_ptr<global_env> &env)> on_update;

        std::function<void(std::shared_ptr<global_env> &env)> on_save;
        std::function<void(std::shared_ptr<global_env> &env)> on_load;
    };

    struct node_event : event
    {
        std::function<bool(std::shared_ptr<global_env> &env)> on_execute;
    };

    int node_next_id = 1;

    int gen_next_id()
    {
        return node_next_id++;
    }

    ed::NodeId gen_next_node_id()
    {
        return gen_next_id();
    }

    ed::LinkId gen_next_link_id()
    {
        return gen_next_id();
    }

    ed::PinId gen_next_pin_id()
    {
        return gen_next_id();
    }

    auto get_base_ptr()
    {
        return shared_from_this();
    }
};

struct link;

struct port
{
    enum class port_type
    {
        in,
        out
    };

    // 全局唯一ID : 用于全局检索
    ed::PinId uuid;
    // 所属节点 : 用于直接访问节点
    std::weak_ptr<base_node> as_node_ptr;
    // 名称 : 调试时使用
    std::string name;
    // 显示文本 : 用于显示
    std::string label;
    // 输入端口值类型 : 用于判断输入端口的值类型
    global_env::value_type type;
    // 端口控制类型 : 用于判断端口的输入输出类型
    const port_type control_type;

    // 构造函数
    port(ed::PinId id, std::weak_ptr<base_node> node, const std::string &name, const std::string &label, global_env::value_type type, port_type control_type)
        : uuid(id), as_node_ptr(node), name(name), label(label), type(type), control_type(control_type)
    {
    }

    global_env::port_ui ui;

    virtual bool has_value() { return false; }
};

struct in_port : port
{
    enum class source_type
    {
        // 从端口本身获取值或者来自其他节点的输出端口
        self_define_or_other_node,
        // 仅从其他节点的输出端口获取值
        only_other_node_no_self,
    };
    // 默认端口值源 : 优先选择外部输入端口的值
    // 两种值的来源，一个是端口本身可设置的值，一个是其他节点的输出端口的值
    source_type default_source;
    // 端口本身参数化值 : 用于设置端口本身的值
    std::optional<global_env::port_value_t> self_value_opt;
    // 输入端口值的引用 : 用来指向别的节点的输出端口的具体值
    std::optional<std::reference_wrapper<global_env::port_value_t>> link_value_opt;
    // 端口所属的连线
    std::optional<std::weak_ptr<link>> which_link_opt;

    std::function<void(std::shared_ptr<base_node>)> callback_event_check_all_input_available;
    // 输入节点引用值变更事件
    void event_ref_value_changed()
    {
        // 调用节点的检测全部输入可用性函数，如果全部输入可用，则调用节点的执行函数
        if (callback_event_check_all_input_available)
            callback_event_check_all_input_available(as_node_ptr.lock());
    }

    // 构造函数
    in_port(ed::PinId id, std::weak_ptr<base_node> node, const std::string &name, const std::string &label, global_env::value_type type, source_type default_source, std::optional<global_env::port_value_t> self_value_opt)
        : port{id, node, name, label, type, port_type::in}, default_source(default_source), self_value_opt(self_value_opt)
    {
    }

    /// @brief 返回是否存在可用的值。线程不安全。
    /// @return 如果端口源是自身，则始终为真。如果端口源是其他节点，则返回是否有连接引用。
    bool has_value() override
    {
        if (default_source == source_type::self_define_or_other_node)
            return true;
        return link_value_opt.has_value();
    }

    bool has_link()
    {
        if (default_source == source_type::only_other_node_no_self)
            return link_value_opt.has_value();
        return false;
    }

    /// @brief
    /// @return
    bool has_self()
    {
        // 如果端口源是仅外部节点，则返回假
        return default_source == source_type::only_other_node_no_self ? false : true;
    }

    auto &value_ref()
    {
        if (default_source == source_type::self_define_or_other_node)
            if (link_value_opt.has_value())
                return link_value_opt.value().get();
            else
                return self_value_opt.value();
        return link_value_opt.value().get();
    }

    /// @brief 获取端口值。
    /// @tparam T 端口值类型，可选列表见 value_type。
    /// @param value 用于接收端口值的引用。
    /// @return 如果端口值类型与模板类型不匹配或者端口没有值，则返回假。
    template <typename T>
    bool get(T &value)
    {
        if (has_value())
            return false;
        if (auto it = global_env::type_map.find(typeid(T).hash_code());
            it != global_env::type_map.end() && it->second == type)
        {
            value = std::get<T>(value_ref());
            return true;
        }
        return false;
    }

    /// @brief 设置自身端口值。
    /// @tparam T 端口值类型，可选列表见 value_type。
    /// @param value 用于设置端口值的值。
    /// @return 如果端口值类型与模板类型不匹配或者端口没有值，则返回假。
    template <typename T>
    bool set_self(const T &value)
    {
        if (default_source != source_type::only_other_node_no_self)
            return false;
        if (auto it = global_env::type_map.find(typeid(T).hash_code());
            it != global_env::type_map.end() && it->second == type)
        {
            self_value_opt = value;
            return true;
        }
        return false;
    }
};

struct out_port : port, std::enable_shared_from_this<out_port>
{
    // 输出端口值 : 用于存储输出端口的值
    global_env::port_value_t value;
    // 输出端点所属的多个连线
    std::vector<std::weak_ptr<link>> which_links;

    // 完全异步：输出节点变更事件
    void event_value_changed()
    {
        // 需要通知所有的连接到该输出端口的输入端口
        // 并且是异步通知，要么持有信号量，要么使用异步线程
        // 持有信号量需要在绑定时进行，异步线程则涉及到线程析构问题
        // 先尝试异步线程

        static auto notifier_func = [&](auto &out_port)
        {
            for (auto &link : out_port->which_links)
                if (auto link_ptr = link.lock(); link_ptr)
                    link_ptr->which_to_in_port.lock()->event_ref_value_changed();
        };

        std::thread t(notifier_func, shared_from_this());
        t.detach();
    }

    // 构造函数
    out_port(ed::PinId id, std::weak_ptr<base_node> node, const std::string &name, const std::string &label, global_env::value_type type) : port{id, node, name, label, type, port_type::out}
    {
    }

    void bind_to_in_port(in_port &in)
    {
        in.link_value_opt = std::ref(value);
    }

    bool has_value() override
    {
        return value.valueless_by_exception();
    }

    /// @brief 获取端口值。
    /// @tparam T 端口值类型，可选列表见 value_type。
    /// @param value 用于接收端口值的引用。
    /// @return 如果端口值类型与模板类型不匹配或者端口没有值，则返回假。
    template <typename T>
    bool get(T &value)
    {
        if (has_value())
            return false;
        if (auto it = global_env::type_map.find(typeid(T).hash_code());
            it != global_env::type_map.end() && it->second == type)
        {
            value = std::get<T>(this->value);
            return true;
        }
        return false;
    }

    /// @brief 设置自身端口值。
    /// @tparam T 端口值类型，可选列表见 value_type。
    /// @param value 用于设置端口值的值。
    /// @return 如果端口值类型与模板类型不匹配或者端口没有值，则返回假。
    template <typename T>
    bool set(const T &value)
    {
        if (auto it = global_env::type_map.find(typeid(T).hash_code());
            it != global_env::type_map.end() && it->second == type)
        {
            auto is_changed = !global_env::is_equal(this->value, value);
            this->value = value;
            if (is_changed)
                event_value_changed();
            return true;
        }
        return false;
    }
};

struct link
{
    // 全局唯一ID : 用于全局检索
    ed::LinkId uuid;
    // 名称 : 调试时使用
    std::string name;
    // 显示文本 : 用于显示
    std::string label;
    // 连线所属的输入端口
    std::weak_ptr<in_port> which_to_in_port;
    // 连线所属的输出端口
    std::weak_ptr<out_port> which_from_out_port;

    // 用来通知传递触发信号
    std::condition_variable cv;

    // 构造函数
    link(ed::LinkId id, const std::string &name, const std::string &label, std::shared_ptr<in_port> to, std::shared_ptr<out_port> from) : uuid(id), name(name), label(label), which_to_in_port(to), which_from_out_port(from)
    {
        from->bind_to_in_port(*to);
    }

    global_env::link_ui ui;
};

struct base_node
{
    struct base_error
    {
        enum class error_source
        {
            node,
            port,
            link,
            other
        };

        std::string message;
        error_source source;
    };
    // 全局唯一ID : 用于全局检索
    ed::NodeId uuid;
    // 名称 : 调试时使用
    std::string name;
    // 显示文本 : 用于显示
    std::string label;
    // 节点类型组 : 用于判断节点分类
    global_env::node_type_group group_type;
    // 节点类型 : 用于判断节点子类型
    std::string node_type;
    // 节点所属的输入端口
    std::vector<std::shared_ptr<in_port>> in_ports;
    // 节点所属的输出端口
    std::vector<std::shared_ptr<out_port>> out_ports;
    // 节点所属的连线
    std::vector<std::weak_ptr<link>> whick_links;
    // 节点工作环境
    std::shared_ptr<global_env> env;
    // 节点异步工作线程对象
    std::optional<std::future<void>> async_worker;
    // 节点错误信息
    std::optional<base_error> last_error_opt;
    // 输出节点变更事件
    std::function<void(std::shared_ptr<out_port> &out_port)> event_on_output_changed = [&](std::shared_ptr<out_port> &out_port)
    {
        for (auto &link : out_port->which_links)
            if (auto link_ptr = link.lock(); link_ptr)
                ; // link_ptr->which_to_in_port.lock()->as_node_ptr.lock()->event.on_update(env);
    };
    // 输入节点变更事件

    global_env::base_node_ui ui;
    global_env::node_event event;

    /// @brief 获取输入端口。
    /// @param name 输入端口名称。
    /// @return 如果找到输入端口，则返回输入端口的指针，否则返回空指针。
    std::shared_ptr<in_port> get_in_port(const std::string &name)
    {
        for (auto &in : in_ports)
            if (in->name == name)
                return in;
        return nullptr;
    }

    /// @brief 获取输出端口。
    /// @param name 输出端口名称。
    /// @return 如果找到输出端口，则返回输出端口的指针，否则返回空指针。
    std::shared_ptr<out_port> get_out_port(const std::string &name)
    {
        for (auto &out : out_ports)
            if (out->name == name)
                return out;
        return nullptr;
    }

    /// @brief 获取连线。
    /// @param name 连线名称。
    /// @return 如果找到连线，则返回连线的指针，否则返回空指针。
    std::shared_ptr<link> get_link(ed::LinkId id)
    {
        for (auto &link : whick_links)
            if (link.lock()->uuid == id)
                return link.lock();
        return nullptr;
    }
};

struct image_flow_env : global_env
{
    std::vector<std::shared_ptr<base_node>> nodes;
    std::vector<std::shared_ptr<link>> links;

    std::shared_ptr<base_node> find_node(ed::NodeId id)
    {
        for (auto &node : nodes)
            if (node->uuid == id)
                return node;
        return nullptr;
    }

    std::shared_ptr<link> find_link(ed::LinkId id)
    {
        for (auto &link : links)
            if (link->uuid == id)
                return link;
        return nullptr;
    }

    std::shared_ptr<in_port> find_in_port(ed::PinId id)
    {
        for (auto &node : nodes)
            for (auto &in : node->in_ports)
                if (in->uuid == id)
                    return in;
        return nullptr;
    }

    std::shared_ptr<out_port> find_out_port(ed::PinId id)
    {
        for (auto &node : nodes)
            for (auto &out : node->out_ports)
                if (out->uuid == id)
                    return out;
        return nullptr;
    }

    std::shared_ptr<port> find_port(ed::PinId id)
    {
        auto in = find_in_port(id);
        if (in)
            return in;
        auto out = find_out_port(id);
        if (out)
            return out;
        return nullptr;
    }

    bool is_pin_linked(ed::PinId id)
    {
        for (auto &link : links)
            if (link->which_to_in_port.lock()->uuid == id || link->which_from_out_port.lock()->uuid == id)
                return true;
        return false;
    }

    bool connect(std::shared_ptr<global_env> env, std::shared_ptr<base_node> from_node, const int &from_out_port_index, std::shared_ptr<base_node> to_node, const int &to_in_port_index)
    {
        auto from = from_node->out_ports[from_out_port_index];
        auto to = to_node->in_ports[to_in_port_index];
        if (from && to)
        {

            std::string name = "link";
            std::string label = env->enum_to_string(from->type) + " to " + env->enum_to_string(to->type);
            auto link_ptr = std::make_shared<link>(env->gen_next_link_id(), name, label, to, from);
            from->which_links.push_back(link_ptr);
            to->which_link_opt = link_ptr;
            links.push_back(link_ptr);
            return true;
        }
        return false;
    }

    std::shared_ptr<base_node> create_node(const std::string &name, const std::string &label, node_type_group group_type, std::string node_type)
    {
        for (auto &[type, factorys] : node_builder_factories[group_type])
        {
            if (type == node_type)
                return factorys(shared_from_this());
        }
        return nullptr;
    }
};

#endif // BASE_NODE_HPP