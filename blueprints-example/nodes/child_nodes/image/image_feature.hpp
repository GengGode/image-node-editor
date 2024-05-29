#pragma once

#include "base_nodes.hpp"
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

// SIFT特征提取
Node *Spawn_ImageFeature_GenerateSIFTFeature(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app)
{
    m_Nodes.emplace_back(GetNextId(), "SIFT特征点提取");
    auto &node = m_Nodes.back();
    node.Type = NodeType::ImageOperation_Feature;
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

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int nfeatures = 0;
        get_value(graph, node->Inputs[1], nfeatures);
        int nOctaveLayers = 3;
        get_value(graph, node->Inputs[2], nOctaveLayers);
        float contrastThreshold = 0.04f;
        get_value(graph, node->Inputs[3], contrastThreshold);
        float edgeThreshold = 10;
        get_value(graph, node->Inputs[4], edgeThreshold);
        float sigma = 1.6f;
        get_value(graph, node->Inputs[5], sigma);
        bool enable_precise_upscale = false;
        get_value(graph, node->Inputs[6], enable_precise_upscale);

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
    node.Type = NodeType::ImageOperation_Feature;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    // double hessianThreshold = 100, int nOctaves = 4, int nOctaveLayers = 3,
    //     bool extended = false, bool upright = false
    node.Inputs.emplace_back(GetNextId(), "Hessian阈值", PinType::Float, 100);
    node.Inputs.emplace_back(GetNextId(), "金字塔层数", PinType::Int, 4);
    node.Inputs.emplace_back(GetNextId(), "每层层数", PinType::Int, 3);
    node.Inputs.emplace_back(GetNextId(), "是否扩展", PinType::Bool, false);
    node.Inputs.emplace_back(GetNextId(), "是否直立", PinType::Bool, false);

    node.Outputs.emplace_back(GetNextId(), "特征数据", PinType::Feature);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        float hessianThreshold = 100;
        get_value(graph, node->Inputs[1], hessianThreshold);
        int nOctaves = 4;
        get_value(graph, node->Inputs[2], nOctaves);
        int nOctaveLayers = 3;
        get_value(graph, node->Inputs[3], nOctaveLayers);
        bool extended = false;
        get_value(graph, node->Inputs[4], extended);
        bool upright = false;
        get_value(graph, node->Inputs[5], upright);

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
    node.Type = NodeType::ImageOperation_Feature;
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

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        int nfeatures = 500;
        get_value(graph, node->Inputs[1], nfeatures);

        float scaleFactor = 1.2f;
        get_value(graph, node->Inputs[2], scaleFactor);

        int nlevels = 8;
        get_value(graph, node->Inputs[3], nlevels);

        int edgeThreshold = 31;
        get_value(graph, node->Inputs[4], edgeThreshold);

        int firstLevel = 0;
        get_value(graph, node->Inputs[5], firstLevel);

        int WTA_K = 2;
        get_value(graph, node->Inputs[6], WTA_K);

        int scoreType = cv::ORB::HARRIS_SCORE;
        get_value(graph, node->Inputs[7], scoreType);

        int patchSize = 31;
        get_value(graph, node->Inputs[8], patchSize);

        int fastThreshold = 20;
        get_value(graph, node->Inputs[9], fastThreshold);

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
    node.Type = NodeType::ImageOperation_Feature;
    node.Inputs.emplace_back(GetNextId(), "图像", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "特征数据", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "颜色", PinType::Color, cv::Scalar(0, 255, 0));
    node.Inputs.emplace_back(GetNextId(), "半径", PinType::Int, 2);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image;
        get_value(graph, node->Inputs[0], image);

        Feature feature;
        get_value(graph, node->Inputs[1], feature);

        cv::Scalar color;
        get_value(graph, node->Inputs[2], color);

        int radius = 2;
        get_value(graph, node->Inputs[3], radius);

        try_catch_block
        {
            cv::Mat result = image.clone();
            cv::drawKeypoints(image, feature.first, result, color, cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

            node->Outputs[0].SetValue(result);
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
    node.Type = NodeType::ImageOperation_Feature;
    node.Inputs.emplace_back(GetNextId(), "特征数据1", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "特征数据2", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "匹配阈值", PinType::Float, 0.7f);

    node.Outputs.emplace_back(GetNextId(), "匹配数据", PinType::Matches);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        Feature feature1, feature2;
        get_value(graph, node->Inputs[0], feature1);

        get_value(graph, node->Inputs[1], feature2);

        float threshold = 0.7f;
        get_value(graph, node->Inputs[2], threshold);

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
    node.Type = NodeType::ImageOperation_Feature;
    node.Inputs.emplace_back(GetNextId(), "图像1", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "图像2", PinType::Image);
    node.Inputs.emplace_back(GetNextId(), "特征数据1", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "特征数据2", PinType::Feature);
    node.Inputs.emplace_back(GetNextId(), "匹配数据", PinType::Matches);

    node.Outputs.emplace_back(GetNextId(), "图像", PinType::Image);

    node.OnExecute = [](Graph *graph, Node *node) -> ExecuteResult
    {
        cv::Mat image1, image2;
        get_value(graph, node->Inputs[0], image1);

        get_value(graph, node->Inputs[1], image2);

        Feature feature1, feature2;
        get_value(graph, node->Inputs[2], feature1);

        get_value(graph, node->Inputs[3], feature2);

        std::vector<cv::DMatch> matches;
        get_value(graph, node->Inputs[4], matches);

        try_catch_block
        {
            cv::Mat result;
            cv::drawMatches(image1, feature1.first, image2, feature2.first, matches, result);

            node->Outputs[0].SetValue(result);
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
