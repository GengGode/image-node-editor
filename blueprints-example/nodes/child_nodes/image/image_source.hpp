#pragma once
#include "base_nodes.hpp"

// window bitblt capture
Node *Spawn_ImageWindowBitbltCapture(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// window graphic capture
Node *Spawn_ImageWindowGraphicCapture(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// local images from dir
Node *Spawn_ImageLocalImagesFromDir(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);

Node *Spawn_ImageFileSource(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);
// load raw image file
Node *Spawn_ImageRawFileSource(const std::function<int()> &GetNextId, const std::function<void(Node *)> &BuildNode, std::vector<Node> &m_Nodes, Application *app);

static NodeWorldGlobal::FactoryGroupFunc_t ImageSourceNodes = {
    {"窗口原生截图", Spawn_ImageWindowBitbltCapture},
    {"窗口图形截图", Spawn_ImageWindowGraphicCapture},
    {"本地图片列表", Spawn_ImageLocalImagesFromDir},
    {"图像文件源", Spawn_ImageFileSource},
    {"图像Raw数据源", Spawn_ImageRawFileSource},
};