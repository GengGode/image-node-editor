#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <application.h>

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <variant>
#include <optional>
#include <atomic>
#include <future>

#include <opencv2/opencv.hpp>

#include <json.hpp>

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

struct NodeIdLess
{
    bool operator()(const ed::NodeId &lhs, const ed::NodeId &rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

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
typedef std::optional<std::any> Object;

typedef std::variant<int, float, bool, std::string,
                     cv::Mat, cv::Rect, cv::Size, cv::Point, cv::Scalar,
                     Contour, Contours,
                     cv::KeyPoint, KeyPoints, Feature, cv::DMatch, Matches,
                     Circles,
                     Object>
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
    {typeid(Object).hash_code(), PinType::Object},
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
    {PinType::Object, "任意对象"},
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

template <>
static bool is_equal(const Object &lft, const Object &rht)
{
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

struct PortValueSerializer
{
    json::value operator()(const ImVec2 &v) const
    {
        return json::object{{"ImVec2", json::array{v.x, v.y}}};
    }
    json::value operator()(const ImColor &v) const
    {
        return json::object{{"ImColor", json::array{v.Value.x, v.Value.y, v.Value.z, v.Value.w}}};
    }
    json::value operator()(const cv::Rect &v) const
    {
        return json::object{{"cv::Rect", json::array{v.x, v.y, v.width, v.height}}};
    }
    json::value operator()(const cv::Size &v) const
    {
        return json::object{{"cv::Size", json::array{v.width, v.height}}};
    }
    json::value operator()(const cv::Point &v) const
    {
        return json::object{{"cv::Point", json::array{v.x, v.y}}};
    }
    json::value operator()(const cv::Scalar &v) const
    {
        return json::object{{"cv::Scalar", json::array{v[0], v[1], v[2], v[3]}}};
    }
    json::value operator()(const Contour &v) const
    {
        json::array arr;
        for (auto point : v)
        {
            arr.push_back(json::array{point.x, point.y});
        }
        return json::object{{"Contour", arr}};
    }
    json::value operator()(const Contours &v) const
    {
        json::array arr;
        for (auto contour : v)
        {
            json::array contour_arr;
            for (auto point : contour)
            {
                contour_arr.push_back(json::array{point.x, point.y});
            }
            arr.push_back(contour_arr);
        }
        return json::object{{"Contours", arr}};
    }
    json::value operator()(const cv::KeyPoint &v) const
    {
        return json::object{{"cv::KeyPoint", json::array{v.pt.x, v.pt.y, v.size, v.angle, v.response, v.octave, v.class_id}}};
    }
    json::value operator()(const KeyPoints &v) const
    {
        json::array arr;
        for (auto keypoint : v)
        {
            arr.push_back(json::array{keypoint.pt.x, keypoint.pt.y, keypoint.size, keypoint.angle, keypoint.response, keypoint.octave, keypoint.class_id});
        }
        return json::object{{"KeyPoints", arr}};
    }
    json::value operator()(const Feature &v) const
    {
        json::array keypoint_arr;
        for (auto keypoint : v.first)
        {
            keypoint_arr.push_back(json::array{keypoint.pt.x, keypoint.pt.y, keypoint.size, keypoint.angle, keypoint.response, keypoint.octave, keypoint.class_id});
        }
        return json::object{{"Feature", json::array{keypoint_arr, "cv::Mat"}}};
    }
    json::value operator()(const cv::DMatch &v) const
    {
        return json::object{{"cv::DMatch", json::array{v.queryIdx, v.trainIdx, v.imgIdx, v.distance}}};
    }
    json::value operator()(const Matches &v) const
    {
        json::array arr;
        for (auto match : v)
        {
            arr.push_back(json::array{match.queryIdx, match.trainIdx, match.imgIdx, match.distance});
        }
        return json::object{{"Matches", arr}};
    }
    json::value operator()(const Circles &v) const
    {
        json::array arr;
        for (auto circle : v)
        {
            arr.push_back(json::array{circle[0], circle[1], circle[2]});
        }
        return json::object{{"Circles", arr}};
    }

    json::value operator()(const port_value_t &v) const
    {
        if (std::holds_alternative<int>(v))
        {
            return json::object{{"int", std::get<int>(v)}};
        }
        if (std::holds_alternative<float>(v))
        {
            return json::object{{"float", std::get<float>(v)}};
        }
        if (std::holds_alternative<bool>(v))
        {
            return json::object{{"bool", std::get<bool>(v)}};
        }
        if (std::holds_alternative<std::string>(v))
        {
            return json::object{{"string", std::get<std::string>(v)}};
        }
        if (std::holds_alternative<cv::Mat>(v))
        {
            return json::object{{"cv::Mat", "cv::Mat"}};
        }
        if (std::holds_alternative<cv::Rect>(v))
        {
            return json::object{{"cv::Rect", json::serialize(std::get<cv::Rect>(v), *this)}};
        }
        if (std::holds_alternative<cv::Size>(v))
        {
            return json::object{{"cv::Size", json::serialize(std::get<cv::Size>(v), *this)}};
        }
        if (std::holds_alternative<cv::Point>(v))
        {
            return json::object{{"cv::Point", json::serialize(std::get<cv::Point>(v), *this)}};
        }
        if (std::holds_alternative<cv::Scalar>(v))
        {
            return json::object{{"cv::Scalar", json::serialize(std::get<cv::Scalar>(v), *this)}};
        }
        if (std::holds_alternative<Contour>(v))
        {
            return json::object{{"Contour", json::serialize(std::get<Contour>(v), *this)}};
        }
        if (std::holds_alternative<Contours>(v))
        {
            return json::object{{"Contours", json::serialize(std::get<Contours>(v), *this)}};
        }
        if (std::holds_alternative<cv::KeyPoint>(v))
        {
            return json::object{{"cv::KeyPoint", json::serialize(std::get<cv::KeyPoint>(v), *this)}};
        }
        if (std::holds_alternative<KeyPoints>(v))
        {
            return json::object{{"KeyPoints", json::serialize(std::get<KeyPoints>(v), *this)}};
        }
        if (std::holds_alternative<Feature>(v))
        {
            return json::object{{"Feature", json::serialize(std::get<Feature>(v), *this)}};
        }
        if (std::holds_alternative<cv::DMatch>(v))
        {
            return json::object{{"cv::DMatch", json::serialize(std::get<cv::DMatch>(v), *this)}};
        }
        if (std::holds_alternative<Matches>(v))
        {
            return json::object{{"Matches", json::serialize(std::get<Matches>(v), *this)}};
        }
        if (std::holds_alternative<Circles>(v))
        {
            return json::object{{"Circles", json::serialize(std::get<Circles>(v), *this)}};
        }
        if (std::holds_alternative<Object>(v))
        {
            return json::object{{"Object", "Object"}};
        }
        return json::object{{"null", "null"}};
    }
};

struct PortValueDeserializer
{
    bool operator()(const json::value &json, ImVec2 &v) const
    {
        if (json.is_object() && json.as_object().contains("ImVec2"))
        {
            auto arr = json.as_object().at("ImVec2").as_array();
            v.x = arr[0].as_float();
            v.y = arr[1].as_float();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, ImColor &v) const
    {
        if (json.is_object() && json.as_object().contains("ImColor"))
        {
            auto arr = json.as_object().at("ImColor").as_array();
            v.Value.x = arr[0].as_float();
            v.Value.y = arr[1].as_float();
            v.Value.z = arr[2].as_float();
            v.Value.w = arr[3].as_float();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::Rect &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::Rect"))
        {
            auto arr = json.as_object().at("cv::Rect").as_array();
            v.x = arr[0].as_integer();
            v.y = arr[1].as_integer();
            v.width = arr[2].as_integer();
            v.height = arr[3].as_integer();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::Size &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::Size"))
        {
            auto arr = json.as_object().at("cv::Size").as_array();
            v.width = arr[0].as_integer();
            v.height = arr[1].as_integer();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::Point &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::Point"))
        {
            auto arr = json.as_object().at("cv::Point").as_array();
            v.x = arr[0].as_integer();
            v.y = arr[1].as_integer();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::Scalar &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::Scalar"))
        {
            auto arr = json.as_object().at("cv::Scalar").as_array();
            v[0] = arr[0].as_float();
            v[1] = arr[1].as_float();
            v[2] = arr[2].as_float();
            v[3] = arr[3].as_float();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Contour &v) const
    {
        if (json.is_object() && json.as_object().contains("Contour"))
        {
            auto arr = json.as_object().at("Contour").as_array();
            for (auto point_json : arr)
            {
                cv::Point point;
                if (operator()(point_json, point))
                {
                    v.push_back(point);
                }
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Contours &v) const
    {
        if (json.is_object() && json.as_object().contains("Contours"))
        {
            auto arr = json.as_object().at("Contours").as_array();
            for (auto contour : arr)
            {
                Contour c;
                if (operator()(contour, c))
                {
                    v.push_back(c);
                }
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::KeyPoint &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::KeyPoint"))
        {
            auto arr = json.as_object().at("cv::KeyPoint").as_array();
            v.pt.x = arr[0].as_float();
            v.pt.y = arr[1].as_float();
            v.size = arr[2].as_float();
            v.angle = arr[3].as_float();
            v.response = arr[4].as_float();
            v.octave = arr[5].as_integer();
            v.class_id = arr[6].as_integer();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, KeyPoints &v) const
    {
        if (json.is_object() && json.as_object().contains("KeyPoints"))
        {
            auto arr = json.as_object().at("KeyPoints").as_array();
            for (auto keypoint : arr)
            {
                cv::KeyPoint kp;
                if (operator()(keypoint, kp))
                {
                    v.push_back(kp);
                }
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Feature &v) const
    {
        if (json.is_object() && json.as_object().contains("Feature"))
        {
            auto arr = json.as_object().at("Feature").as_array();
            KeyPoints keypoint;
            if (operator()(arr[0], keypoint))
            {
                v.first = keypoint;
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, cv::DMatch &v) const
    {
        if (json.is_object() && json.as_object().contains("cv::DMatch"))
        {
            auto arr = json.as_object().at("cv::DMatch").as_array();
            v.queryIdx = arr[0].as_integer();
            v.trainIdx = arr[1].as_integer();
            v.imgIdx = arr[2].as_integer();
            v.distance = arr[3].as_float();
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Matches &v) const
    {
        if (json.is_object() && json.as_object().contains("Matches"))
        {
            auto arr = json.as_object().at("Matches").as_array();
            for (auto match : arr)
            {
                cv::DMatch m;
                if (operator()(match, m))
                {
                    v.push_back(m);
                }
            }
            return true;
        }
        return false;
    }
    bool operator()(const json::value &json, Circles &v) const
    {
        if (json.is_object() && json.as_object().contains("Circles"))
        {
            auto arr = json.as_object().at("Circles").as_array();
            for (auto circle : arr)
            {
                v.push_back({circle[0].as_float(), circle[1].as_float(), circle[2].as_float()});
            }
            return true;
        }
        return false;
    }

    bool operator()(const json::value &json, port_value_t &v) const
    {
        if (json.is_object())
        {
            if (json.as_object().contains("int"))
            {
                v = json.as_object().at("int").as_integer();
                return true;
            }
            if (json.as_object().contains("float"))
            {
                v = json.as_object().at("float").as_float();
                return true;
            }
            if (json.as_object().contains("bool"))
            {
                v = json.as_object().at("bool").as_boolean();
                return true;
            }
            if (json.as_object().contains("string"))
            {
                v = json.as_object().at("string").as_string();
                return true;
            }
            if (json.as_object().contains("cv::Mat"))
            {
                v = cv::Mat();
                return true;
            }
            if (json.as_object().contains("cv::Rect"))
            {
                cv::Rect rect;
                if (operator()(json.as_object().at("cv::Rect"), rect))
                {
                    v = rect;
                    return true;
                }
            }
            if (json.as_object().contains("cv::Size"))
            {
                cv::Size size;
                if (operator()(json.as_object().at("cv::Size"), size))
                {
                    v = size;
                    return true;
                }
            }
            if (json.as_object().contains("cv::Point"))
            {
                cv::Point point;
                if (operator()(json.as_object().at("cv::Point"), point))
                {
                    v = point;
                    return true;
                }
            }
            if (json.as_object().contains("cv::Scalar"))
            {
                cv::Scalar scalar;
                if (operator()(json.as_object().at("cv::Scalar"), scalar))
                {
                    v = scalar;
                    return true;
                }
            }
            if (json.as_object().contains("Contour"))
            {
                Contour contour;
                if (operator()(json.as_object().at("Contour"), contour))
                {
                    v = contour;
                    return true;
                }
            }
            if (json.as_object().contains("Contours"))
            {
                Contours contours;
                if (operator()(json.as_object().at("Contours"), contours))
                {
                    v = contours;
                    return true;
                }
            }
            if (json.as_object().contains("cv::KeyPoint"))
            {
                cv::KeyPoint keypoint;
                if (operator()(json.as_object().at("cv::KeyPoint"), keypoint))
                {
                    v = keypoint;
                    return true;
                }
            }
            if (json.as_object().contains("KeyPoints"))
            {
                KeyPoints keypoints;
                if (operator()(json.as_object().at("KeyPoints"), keypoints))
                {
                    v = keypoints;
                    return true;
                }
            }
            if (json.as_object().contains("Feature"))
            {
                Feature feature;
                if (operator()(json.as_object().at("Feature"), feature))
                {
                    v = feature;
                    return true;
                }
            }
            if (json.as_object().contains("cv::DMatch"))
            {
                cv::DMatch match;
                if (operator()(json.as_object().at("cv::DMatch"), match))
                {
                    v = match;
                    return true;
                }
            }
            if (json.as_object().contains("Matches"))
            {
                Matches matches;
                if (operator()(json.as_object().at("Matches"), matches))
                {
                    v = matches;
                    return true;
                }
            }
            if (json.as_object().contains("Circles"))
            {
                Circles circles;
                if (operator()(json.as_object().at("Circles"), circles))
                {
                    v = circles;
                    return true;
                }
            }
            if (json.as_object().contains("Object"))
            {
                v = std::any();
                return true;
            }
        }
        return false;
    }
};