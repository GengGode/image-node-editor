#pragma once
#include "base_nodes.hpp"

#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif

#include "../../utilities/convert.string.h"

namespace window_scale
{
    namespace window_last_version
    {
        static double get_screen_scale(HWND current_handle = nullptr)
        {
            auto handle = current_handle;
            if (handle == nullptr)
                handle = GetDesktopWindow();
            HMONITOR monitor_handle = MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST);

            MONITORINFOEX miex;
            miex.cbSize = sizeof(miex);
            GetMonitorInfo(monitor_handle, &miex);
            int screen_x_logical = (miex.rcMonitor.right - miex.rcMonitor.left);

            DEVMODE dm;
            dm.dmSize = sizeof(dm);
            dm.dmDriverExtra = 0;
            EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
            int screen_x_physical = dm.dmPelsWidth;

            double screen_scale = ((double)screen_x_physical / (double)screen_x_logical);
            return screen_scale;
        }
    } // namespace window_last_version

    namespace window_current_version
    {
        static double get_screen_scale(HWND current_handle = nullptr)
        {
            auto handle = current_handle;
            if (handle == nullptr)
                handle = GetDesktopWindow();
            return GetDpiForWindow(handle) / 96.0;
        }
    } // namespace window_current_version

    namespace window_first_version
    {
        static double get_screen_scale(HWND current_handle = nullptr)
        {
            auto handle = current_handle;
            if (handle == nullptr)
                handle = GetDesktopWindow();
            HDC hdc = GetDC(handle);
            int x = GetDeviceCaps(hdc, DESKTOPHORZRES);
            int screen_x = GetSystemMetrics(SM_CXSCREEN);
            double scale = (double)x / (double)screen_x;
            return scale;
        }
    } // namespace window_first_version

#if defined(_WIN32) || defined(_WIN64)
#if defined(_MSC_VER)
#if _MSC_VER >= 1920
#define WINDOW_VERSION window_last_version
#elif _MSC_VER >= 1910
#define WINDOW_VERSION window_current_version
#else
#define WINDOW_VERSION window_first_version
#endif
#else
#define WINDOW_VERSION window_first_version
#endif
#else
#define WINDOW_VERSION window_first_version
#endif
    using namespace WINDOW_VERSION;
} // namespace tianli::frame::capture::utils::window_scale
// window bitblt capture
Node *Spawn_ImageWindowBitbltCapture(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "窗口原生截图");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageSource;
    node.Inputs.emplace_back(GetNextId(), "窗口名称", PinType::String, std::string());
    node.Inputs.emplace_back(GetNextId(), "窗口类名", PinType::String, std::string());
    node.Outputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string window;
        get_value(graph, node->Inputs[0], window);
        std::string class_name;
        get_value(graph, node->Inputs[1], class_name);

        node->Inputs[0].Value = window;
        node->Inputs[1].Value = class_name;

        try_catch_block;

        auto w_class_name = utils::to_wstring(class_name);
        auto w_window = utils::to_wstring(window);
        const wchar_t *w_window_name = w_window.size() > 0 ? w_window.c_str() : nullptr;
        const wchar_t *w_class_name_str = w_class_name.size() > 0 ? w_class_name.c_str() : nullptr;

        cv::Mat image;

        HWND handle = FindWindowW(w_class_name_str, w_window_name);
        if (handle == NULL)
            return ExecuteResult::ErrorNode(node->ID, "未找到窗口");

        RECT rect = {0, 0, 0, 0};
        if (GetWindowRect(handle, &rect) == false)
            return ExecuteResult::ErrorNode(node->ID, "获取窗口位置失败");
        RECT client_rect = {0, 0, 0, 0};
        if (GetClientRect(handle, &client_rect) == false)
            return ExecuteResult::ErrorNode(node->ID, "获取窗口客户区位置失败");
        double screen_scale = window_scale::get_screen_scale(handle);
        cv::Size client_size = {0, 0};
        client_size.width = (int)(screen_scale * (client_rect.right - client_rect.left));
        client_size.height = (int)(screen_scale * (client_rect.bottom - client_rect.top));
        HDC hdc = GetDC(handle);
        HDC hmemdc = CreateCompatibleDC(hdc);
        HBITMAP hbitmap = CreateCompatibleBitmap(hdc, client_size.width, client_size.height);
        SelectObject(hmemdc, hbitmap);
        BitBlt(hmemdc, 0, 0, client_size.width, client_size.height, hdc, 0, 0, SRCCOPY);
        DeleteDC(hmemdc);
        ReleaseDC(handle, hdc);
        BITMAP source_bitmap;
        GetObject(hbitmap, sizeof(BITMAP), &source_bitmap);
        int nChannels = source_bitmap.bmBitsPixel == 1 ? 1 : source_bitmap.bmBitsPixel / 8;
        image.create(cv::Size(source_bitmap.bmWidth, source_bitmap.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
        GetBitmapBits(hbitmap, source_bitmap.bmHeight * source_bitmap.bmWidth * nChannels, image.data);
        image = image(cv::Rect(client_rect.left, client_rect.top, client_size.width, client_size.height));

        node->Outputs[0].SetValue(image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

#include "utils/capture.window_graphic.hpp"
struct wgc_node_state_value : node_state_value
{
    tianli::frame::capture::capture_window_graphics capture;
};

// window graphic capture
Node *Spawn_ImageWindowGraphicCapture(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "窗口图形截图");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageSource;
    node.Inputs.emplace_back(GetNextId(), "窗口名称", PinType::String, std::string());
    node.Inputs.emplace_back(GetNextId(), "窗口类名", PinType::String, std::string());
    node.Outputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string window;
        get_value(graph, node->Inputs[0], window);
        std::string class_name;
        get_value(graph, node->Inputs[1], class_name);

        node->Inputs[0].Value = window;
        node->Inputs[1].Value = class_name;

        try_catch_block;

        if (node->state_value == nullptr)
            node->state_value = std::make_shared<wgc_node_state_value>();
        auto &wgc_value = std::static_pointer_cast<wgc_node_state_value>(node->state_value);
        if (wgc_value==nullptr)
            return ExecuteResult::ErrorNode(node->ID, "状态值类型错误");
        auto &capture = wgc_value->capture;

        auto w_class_name = utils::to_wstring(class_name);
        auto w_window = utils::to_wstring(window);
        const wchar_t *w_window_name = w_window.size() > 0 ? w_window.c_str() : nullptr;
        const wchar_t *w_class_name_str = w_class_name.size() > 0 ? w_class_name.c_str() : nullptr;
       
        cv::Mat image;

        HWND handle = FindWindowW(w_class_name_str, w_window_name);
        if (handle == NULL)
            return ExecuteResult::ErrorNode(node->ID, "未找到窗口");

        if(capture.is_initialized==false)
            capture.initialization();

        if(capture.set_capture_handle(handle)==false)
            return ExecuteResult::ErrorNode(node->ID, "设置窗口句柄失败");

        if (capture.get_frame(image) == false)
            return ExecuteResult::ErrorNode(node->ID, "获取图像失败");

        node->Outputs[0].SetValue(image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}


// local images from dir
Node *Spawn_ImageLocalImagesFromDir(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "本地图片列表");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageSource;
    node.Inputs.emplace_back(GetNextId(), "目录", PinType::String, std::string("."));
    node.Inputs.emplace_back(GetNextId(), "下次输出索引", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "是否锁定图片", PinType::Bool, false);
    node.Outputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string dir;
        get_value(graph, node->Inputs[0], dir);
        int index;
        get_value(graph, node->Inputs[1], index);
        bool lock;
        get_value(graph, node->Inputs[2], lock);

        node->Inputs[0].Value = dir;
        node->Inputs[1].Value = index;
        node->Inputs[2].Value = lock;

        try_catch_block;

        std::filesystem::path images_dir(dir);
        if (std::filesystem::exists(images_dir) == false)
            return ExecuteResult::ErrorNode(node->ID, "目录不存在");

        cv::Mat result;
        std::vector<std::filesystem::path> images;
        for (auto &entry : std::filesystem::directory_iterator(images_dir))
        {
            if (entry.is_directory())
                continue;
            if (entry.path().extension() != ".jpg" && entry.path().extension() != ".png")
                continue;
            images.push_back(entry.path());
        }

        if (images.size() > 0)
        {
            index = index % images.size();
            result = cv::imread(images[index].string(), cv::IMREAD_UNCHANGED);
        }

        if (result.empty())
            return ExecuteResult::ErrorNode(node->ID, "未找到图片");

        int next_index = index + (int)(lock ? 0 : 1);

        node->Outputs[0].SetValue(result);
        node->Inputs[1].Value = next_index;
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

Node *Spawn_ImageFileSource(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "图像文件源");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), PinType::String, "图像路径", std::string("resources/texture.png"));
    node.Outputs.emplace_back(GetNextId(), PinType::Image);
    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node)
    {
        std::string path;
        auto result = get_value(graph, node->Inputs[0], path);
        if (result.has_error())
            return result;

        std::filesystem::path p(path);
        if (!std::filesystem::exists(p))
            return ExecuteResult::ErrorNode(node->ID, "文件没有找到");

        try_catch_block;
        cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);
        if (image.empty())
            return ExecuteResult::ErrorNode(node->ID, "图片加载失败");
        node->Outputs[0].SetValue(image);
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageSourceNodes = {
    {"窗口原生截图", Spawn_ImageWindowBitbltCapture},
    {"窗口图形截图", Spawn_ImageWindowGraphicCapture},
    {"本地图片列表", Spawn_ImageLocalImagesFromDir},
    {"图像文件源", Spawn_ImageFileSource},
};