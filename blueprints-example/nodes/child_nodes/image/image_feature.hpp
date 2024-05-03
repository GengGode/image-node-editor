#pragma once

#include "base_nodes.hpp"
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

// SIFT特征提取
Node *Spawn_ImageFeature_GenerateSIFTFeature(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "SIFT特征点提取");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    // int nfeatures = 0, int nOctaveLayers = 3,
    //     double contrastThreshold = 0.04, double edgeThreshold = 10,
    //     double sigma = 1.6, bool enable_precise_upscale = false
    node.Inputs.emplace_back(GetNextId(), "最大特征点数", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "金字塔层数", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "对比度阈值", PinType::Float, 0.04f);
    node.Inputs.emplace_back(GetNextId(), "边缘阈值", PinType::Float, 10);
    node.Inputs.emplace_back(GetNextId(), "高斯核系数", PinType::Float, 1.6f);
    node.Inputs.emplace_back(GetNextId(), "是否启用精确缩放", PinType::Bool, false);

    node.Outputs.emplace_back(GetNextId(), "特征数据", PinType::Feature);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        int nfeatures = 0;
        result = get_value(graph, node->Inputs[1], nfeatures);
        int nOctaveLayers = 3;
        result = get_value(graph, node->Inputs[2], nOctaveLayers);
        float contrastThreshold = 0.04f;
        result = get_value(graph, node->Inputs[3], contrastThreshold);
        float edgeThreshold = 10;
        result = get_value(graph, node->Inputs[4], edgeThreshold);
        float sigma = 1.6f;
        result = get_value(graph, node->Inputs[5], sigma);
        bool enable_precise_upscale = false;
        result = get_value(graph, node->Inputs[6], enable_precise_upscale);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = nfeatures;
        node->Inputs[2].Value = nOctaveLayers;
        node->Inputs[3].Value = contrastThreshold;
        node->Inputs[4].Value = edgeThreshold;
        node->Inputs[5].Value = sigma;
        node->Inputs[6].Value = enable_precise_upscale;

        try_catch_block
        {
            // sift detector
            auto detector = cv::SIFT::create(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma, enable_precise_upscale);

            std::vector<cv::KeyPoint> keypoints;
            cv::Mat descriptors;
            detector->detectAndCompute(image, cv::noArray(), keypoints, descriptors);

            Feature output;
            output.first = keypoints;
            output.second = descriptors;

            node->Outputs[0].SetValue(output);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// SURF特征提取
Node *Spawn_ImageFeature_GenerateSURFFeature(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "SURF特征点提取");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    // double hessianThreshold = 100, int nOctaves = 4, int nOctaveLayers = 3,
    //     bool extended = false, bool upright = false
    node.Inputs.emplace_back(GetNextId(), "Hessian阈值", PinType::Float, 100);
    node.Inputs.emplace_back(GetNextId(), "金字塔层数", PinType::Int, 4);
    node.Inputs.emplace_back(GetNextId(), "每层层数", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "是否扩展", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "是否直立", PinType::Bool, false);

    node.Outputs.emplace_back(GetNextId(), "特征数据", PinType::Feature);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        float hessianThreshold = 100;
        result = get_value(graph, node->Inputs[1], hessianThreshold);
        int nOctaves = 4;
        result = get_value(graph, node->Inputs[2], nOctaves);
        int nOctaveLayers = 3;
        result = get_value(graph, node->Inputs[3], nOctaveLayers);
        bool extended = false;
        result = get_value(graph, node->Inputs[4], extended);
        bool upright = false;
        result = get_value(graph, node->Inputs[5], upright);

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = hessianThreshold;
        node->Inputs[2].Value = nOctaves;
        node->Inputs[3].Value = nOctaveLayers;
        node->Inputs[4].Value = extended;
        node->Inputs[5].Value = upright;

        try_catch_block
        {
            // surf detector
            auto detector = cv::xfeatures2d::SURF::create(hessianThreshold, nOctaves, nOctaveLayers, extended, upright);

            std::vector<cv::KeyPoint> keypoints;
            cv::Mat descriptors;
            detector->detectAndCompute(image, cv::noArray(), keypoints, descriptors);

            Feature output;
            output.first = keypoints;
            output.second = descriptors;

            node->Outputs[0].SetValue(output);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// ORB特征提取
Node *Spawn_ImageFeature_GenerateORBFeature(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "ORB特征点提取");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    // int nfeatures=500, float scaleFactor=1.2f, int nlevels=8, int edgeThreshold=31,
    // int firstLevel = 0, int WTA_K = 2, ORB::ScoreType scoreType = ORB::HARRIS_SCORE, int patchSize = 31, int fastThreshold = 20
    node.Inputs.emplace_back(GetNextId(), "最大特征点数", PinType::Int, 500);
    node.Inputs.emplace_back(GetNextId(), "尺度因子", PinType::Float, 1.2f);
    node.Inputs.emplace_back(GetNextId(), "层数", PinType::Int, 8);
    node.Inputs.emplace_back(GetNextId(), "边缘阈值", PinType::Int, 31);
    node.Inputs.emplace_back(GetNextId(), "第一层", PinType::Int, 0);
    node.Inputs.emplace_back(GetNextId(), "WTA_K", PinType::Int, 2);
    node.Inputs.emplace_back(GetNextId(), "评分类型", PinType::Int, cv::ORB::HARRIS_SCORE);
    node.Inputs.emplace_back(GetNextId(), "块大小", PinType::Int, 31);
    node.Inputs.emplace_back(GetNextId(), "快速阈值", PinType::Int, 20);

    node.Outputs.emplace_back(GetNextId(), "特征数据", PinType::Feature);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        int nfeatures = 500;
        result = get_value(graph, node->Inputs[1], nfeatures);
        if (result.has_error())
            return result;
        float scaleFactor = 1.2f;
        result = get_value(graph, node->Inputs[2], scaleFactor);
        if (result.has_error())
            return result;
        int nlevels = 8;
        result = get_value(graph, node->Inputs[3], nlevels);
        if (result.has_error())
            return result;
        int edgeThreshold = 31;
        result = get_value(graph, node->Inputs[4], edgeThreshold);
        if (result.has_error())
            return result;
        int firstLevel = 0;
        result = get_value(graph, node->Inputs[5], firstLevel);
        if (result.has_error())
            return result;
        int WTA_K = 2;
        result = get_value(graph, node->Inputs[6], WTA_K);
        if (result.has_error())
            return result;
        int scoreType = cv::ORB::HARRIS_SCORE;
        result = get_value(graph, node->Inputs[7], scoreType);
        if (result.has_error())
            return result;
        int patchSize = 31;
        result = get_value(graph, node->Inputs[8], patchSize);
        if (result.has_error())
            return result;
        int fastThreshold = 20;
        result = get_value(graph, node->Inputs[9], fastThreshold);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = nfeatures;
        node->Inputs[2].Value = scaleFactor;
        node->Inputs[3].Value = nlevels;
        node->Inputs[4].Value = edgeThreshold;
        node->Inputs[5].Value = firstLevel;
        node->Inputs[6].Value = WTA_K;
        node->Inputs[7].Value = scoreType;
        node->Inputs[8].Value = patchSize;
        node->Inputs[9].Value = fastThreshold;

        try_catch_block
        {
            // orb detector
            auto detector = cv::ORB::create(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, (cv::ORB::ScoreType)scoreType, patchSize, fastThreshold);

            std::vector<cv::KeyPoint> keypoints;
            cv::Mat descriptors;
            detector->detectAndCompute(image, cv::noArray(), keypoints, descriptors);

            Feature output;
            output.first = keypoints;
            output.second = descriptors;

            node->Outputs[0].SetValue(output);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 绘制特征点
Node *Spawn_ImageFeature_DrawFeaturePoints(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "绘制特征点");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "特征数据", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "颜色", PinType::Color, cv::Scalar(0, 255, 0));
    node.Inputs.emplace_back(GetNextId(), "半径", PinType::Int, 2);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        auto result = get_value(graph, node->Inputs[0], image);
        if (result.has_error())
            return result;
        Feature feature;
        result = get_value(graph, node->Inputs[1], feature);
        if (result.has_error())
            return result;
        cv::Scalar color;
        result = get_value(graph, node->Inputs[2], color);
        if (result.has_error())
            return result;
        int radius = 2;
        result = get_value(graph, node->Inputs[3], radius);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image;
        node->Inputs[1].Value = feature;
        node->Inputs[2].Value = color;
        node->Inputs[3].Value = radius;

        try_catch_block
        {
            cv::drawKeypoints(image, feature.first, image, color, cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

            node->Outputs[0].SetValue(image);
        }
        catch_block_and_return;
    };

    BuildNode(&node);
    return &node;
}

// 匹配特征点
Node *Spawn_ImageFeature_MatchFeaturePoints(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "匹配特征点");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "特征数据1", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "特征数据2", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "匹配阈值", PinType::Float, 0.7f);

    node.Outputs.emplace_back(GetNextId(), "匹配数据", PinType::Matches);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        Feature feature1, feature2;
        auto result = get_value(graph, node->Inputs[0], feature1);
        if (result.has_error())
            return result;
        result = get_value(graph, node->Inputs[1], feature2);
        if (result.has_error())
            return result;
        float threshold = 0.7f;
        result = get_value(graph, node->Inputs[2], threshold);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = feature1;
        node->Inputs[1].Value = feature2;
        node->Inputs[2].Value = threshold;

        try_catch_block
        {
            std::vector<std::vector<cv::DMatch>> matches;
            auto matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
            // matcher->knnMatch(query_fts.descriptors, train_fts.descriptors, match_group, 2);
            matcher->knnMatch(feature1.second, feature2.second, matches, 2);

            std::vector<cv::DMatch> good_matches;
            for (size_t i = 0; i < matches.size(); i++)
            {
                if (matches[i][0].distance < threshold * matches[i][1].distance)
                {
                    good_matches.push_back(matches[i][0]);
                }
            }

            node->Outputs[0].SetValue(good_matches);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

// 绘制匹配特征点
Node *Spawn_ImageFeature_DrawMatchedFeaturePoints(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "绘制匹配特征点");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageFlow;
    node.Inputs.emplace_back(GetNextId(), "图像1", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "图像2", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "特征数据1", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "特征数据2", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "匹配数据", PinType::Matches);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.Outputs[0].app = app;

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image1, image2;
        auto result = get_value(graph, node->Inputs[0], image1);
        if (result.has_error())
            return result;
        result = get_value(graph, node->Inputs[1], image2);
        if (result.has_error())
            return result;
        Feature feature1, feature2;
        result = get_value(graph, node->Inputs[2], feature1);
        if (result.has_error())
            return result;
        result = get_value(graph, node->Inputs[3], feature2);
        if (result.has_error())
            return result;
        std::vector<cv::DMatch> matches;
        result = get_value(graph, node->Inputs[4], matches);
        if (result.has_error())
            return result;

        // Display image
        node->Inputs[0].Value = image1;
        node->Inputs[1].Value = image2;
        node->Inputs[2].Value = feature1;
        node->Inputs[3].Value = feature2;
        node->Inputs[4].Value = matches;

        try_catch_block
        {
            cv::Mat output;
            cv::drawMatches(image1, feature1.first, image2, feature2.first, matches, output);

            node->Outputs[0].SetValue(output);
        }
        catch_block_and_return;
    };

    BuildNode(&node);

    return &node;
}

static NodeWorldGlobal::FactoryGroupFunc_t ImageFeatureNodes = {
    {"SURF特征点提取", Spawn_ImageFeature_GenerateSURFFeature},
    {"SIFT特征点提取", Spawn_ImageFeature_GenerateSIFTFeature},
    {"ORB特征点提取", Spawn_ImageFeature_GenerateORBFeature},
    {"绘制特征点", Spawn_ImageFeature_DrawFeaturePoints},
    {"匹配特征点", Spawn_ImageFeature_MatchFeaturePoints},
    {"绘制匹配特征点", Spawn_ImageFeature_DrawMatchedFeaturePoints},
};
