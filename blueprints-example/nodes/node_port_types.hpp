#pragma once
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
#include <atomic>
#include <future>

#include <opencv2/opencv.hpp>

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

enum class PinKind
{
    Output,
    Input
};

typedef std::vector<cv::Point> Contour;
typedef std::vector<Contour> Contours;
typedef std::vector<cv::KeyPoint> KeyPoints;
typedef std::pair<KeyPoints, cv::Mat> Feature;
typedef std::vector<cv::DMatch> Matches;
typedef std::vector<cv::Vec3f> Circles;

typedef std::variant<int, float, bool, std::string,
                     cv::Mat, cv::Rect, cv::Size, cv::Point, cv::Scalar,
                     Contour, Contours,
                     cv::KeyPoint, KeyPoints, Feature, cv::DMatch, Matches,
                     Circles>
    port_value_t;

enum class PinType
{
    Flow,
    Image,
    Rect,
    Size,
    Point,
    Color,
    Contour,
    Contours,
    KeyPoint,
    KeyPoints,
    Feature,
    Match,
    Matches,
    Circles,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
    Delegate,
};

static const std::map<std::size_t, PinType> typeMap = {
    {typeid(int).hash_code(), PinType::Int},
    {typeid(float).hash_code(), PinType::Float},
    {typeid(bool).hash_code(), PinType::Bool},
    {typeid(std::string).hash_code(), PinType::String},
    {typeid(cv::Mat).hash_code(), PinType::Image},
    {typeid(cv::Rect).hash_code(), PinType::Rect},
    {typeid(cv::Size).hash_code(), PinType::Size},
    {typeid(cv::Point).hash_code(), PinType::Point},
    {typeid(cv::Scalar).hash_code(), PinType::Color},
    {typeid(Contour).hash_code(), PinType::Contour},
    {typeid(Contours).hash_code(), PinType::Contours},
    {typeid(cv::KeyPoint).hash_code(), PinType::KeyPoint},
    {typeid(KeyPoints).hash_code(), PinType::KeyPoints},
    {typeid(Feature).hash_code(), PinType::Feature},
    {typeid(cv::DMatch).hash_code(), PinType::Match},
    {typeid(Matches).hash_code(), PinType::Matches},
    {typeid(Circles).hash_code(), PinType::Circles},
};
static const std::map<PinType, std::string> typeLabelNames = {
    {PinType::Int, "整数"},
    {PinType::Float, "浮点数"},
    {PinType::Bool, "布尔值"},
    {PinType::String, "文本"},
    {PinType::Image, "图像"},
    {PinType::Rect, "矩形范围"},
    {PinType::Size, "大小"},
    {PinType::Point, "坐标点"},
    {PinType::Color, "颜色"},
    {PinType::Contour, "轮廓"},
    {PinType::Contours, "轮廓集合"},
    {PinType::KeyPoint, "关键点"},
    {PinType::KeyPoints, "关键点集合"},
    {PinType::Feature, "特征数据"},
    {PinType::Match, "匹配对"},
    {PinType::Matches, "匹配集合"},
    {PinType::Circles, "霍夫圆数据集合"},
};

template <typename T>
static bool is_equal(const T &lft, const T &rht)
{
    return lft == rht;
}

template <>
static bool is_equal(const cv::Mat &lft, const cv::Mat &rht)
{
    if (lft.empty() && rht.empty())
        return true;
    if (lft.empty() || rht.empty())
        return false;

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

template <>
static bool is_equal(const Contour &lft, const Contour &rht)
{
    if (lft.size() != rht.size())
        return false;
    for (size_t i = 0; i < lft.size(); i++)
    {
        if (lft[i] != rht[i])
            return false;
    }
    return true;
}

template <>
static bool is_equal(const Contours &lft, const Contours &rht)
{
    if (lft.size() != rht.size())
        return false;
    for (size_t i = 0; i < lft.size(); i++)
    {
        if (!is_equal(lft[i], rht[i]))
            return false;
    }
    return true;
}

template <>
static bool is_equal(const cv::KeyPoint &lft, const cv::KeyPoint &rht)
{
    return lft.pt == rht.pt &&
           lft.size == rht.size &&
           lft.angle == rht.angle &&
           lft.response == rht.response &&
           lft.octave == rht.octave &&
           lft.class_id == rht.class_id;
}

template <>
static bool is_equal(const KeyPoints &lft, const KeyPoints &rht)
{
    if (lft.size() != rht.size())
        return false;
    for (size_t i = 0; i < lft.size(); i++)
    {
        if (!is_equal(lft[i], rht[i]))
            return false;
    }
    return true;
}

template <>
static bool is_equal(const Feature &lft, const Feature &rht)
{
    return is_equal(lft.first, rht.first) &&
           is_equal(lft.second, rht.second);
}

template <>
static bool is_equal(const cv::DMatch &lft, const cv::DMatch &rht)
{
    return lft.queryIdx == rht.queryIdx &&
           lft.trainIdx == rht.trainIdx &&
           lft.imgIdx == rht.imgIdx &&
           lft.distance == rht.distance;
}

template <>
static bool is_equal(const Matches &lft, const Matches &rht)
{
    if (lft.size() != rht.size())
        return false;
    for (size_t i = 0; i < lft.size(); i++)
    {
        if (!is_equal(lft[i], rht[i]))
            return false;
    }
    return true;
}

static bool is_equal(const port_value_t &lft, const port_value_t &rht)
{
    return std::visit([&](auto &&arg1, auto &&arg2)
                      { if (typeid(arg1)!= typeid(arg2))
                                return false;
                            return is_equal(arg1, arg2); },
                      lft, rht);
}
