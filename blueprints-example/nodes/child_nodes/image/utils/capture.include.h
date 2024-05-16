#pragma once

#include <Windows.h>

#include <opencv2/core.hpp>

namespace tianli::frame
{
    class frame_source
    {
    public:
        enum class source_type
        {
            unknown,
            bitblt,
            window_graphics,
            video,
            picture
        };
        enum class source_mode
        {
            handle,
            frame
        };

    public:
        source_type type = source_type::unknown;
        source_mode mode = source_mode::handle;
        bool is_initialized = false;
        bool is_callback = false;
        bool has_frame_rect_callback = false;
        std::function<cv::Rect(cv::Rect)> frame_rect_callback;

    public:
        frame_source() = default;
        virtual ~frame_source() = default;
        virtual bool initialization() { return false; }
        virtual bool uninitialized() { return false; }
        virtual bool get_frame(cv::Mat &frame) = 0;
        virtual bool set_capture_handle(HWND handle) = 0;
        virtual bool set_local_frame(cv::Mat frame) = 0;
        virtual bool set_local_file(std::string file) = 0;
        virtual bool set_source_handle_callback(std::function<HWND()> callback) = 0;
        virtual bool set_source_frame_callback(std::function<cv::Mat()> callback) = 0;
        virtual bool set_frame_rect_callback(std::function<cv::Rect(cv::Rect)> callback) = 0;
    };

} // namespace tianli::frame
namespace tianli::frame::capture
{
    class capture_source : public frame_source
    {
    public:
        capture_source() : frame_source() {}
        ~capture_source() override = default;
        bool set_source_frame_callback(std::function<cv::Mat()> callback) override { return false; }

        bool set_local_frame(cv::Mat frame) override { return false; }
        bool set_local_file(std::string file) override { return false; }
        // TODO: frame_source::set_source_frame_callback
        bool set_frame_rect_callback(std::function<cv::Rect(cv::Rect)> callback) override { return false; }

    protected:
        std::function<HWND()> source_handle_callback;
        HWND source_handle = nullptr;
        cv::Mat source_frame;
    };

} // namespace tianli::frame::capture
