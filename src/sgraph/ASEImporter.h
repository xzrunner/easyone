#pragma once

#include <blueprint/typedef.h>
#include <shadergraph/Node.h>

#include <node0/typedef.h>
#include <aseimp/FileLoader.h>
#include <SM_Vector.h>

#include <string>
#include <vector>
#include <map>

namespace eone
{
namespace sgraph
{

class ASEImporter
{
public:
    ASEImporter();

    void LoadAsset(const std::string& filepath);
    void LoadShader(const std::string& filepath);

    auto& GetNodes() const { return m_scene_nodes; }

private:
    void Load(const aseimp::FileLoader& loader);

    void CreateSceneNode(const bp::NodePtr& bp_node, const sm::vec2& pos);

    static int PortTypeASEImpToBP(aseimp::WirePortDataType type);
    static void InputPortASEImpToSG(std::vector<sg::Node::PinsDesc>& dst,
        const std::vector<aseimp::FileLoader::InputPort>& src);
    static void OutputPortASEImpToSG(std::vector<sg::Node::PinsDesc>& dst,
        const std::vector<aseimp::FileLoader::OutputPort>& src);

private:
    struct Node
    {
        Node(const aseimp::FileLoader::Node& src, bp::NodePtr dst) : src(src), dst(dst) {}

        const aseimp::FileLoader::Node& src;
        bp::NodePtr dst;
    };

private:
    std::vector<n0::SceneNodePtr> m_scene_nodes;

    std::map<int, Node> m_map_nodes;

}; // ASEImporter

}
}