#pragma once

#include <blueprint/typedef.h>

#include <node0/typedef.h>

#include <string>
#include <vector>
#include <map>

namespace aseimp { class FileLoader; }

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

    auto& GetNodes() const { return m_nodes; }

private:
    void Load(const aseimp::FileLoader& loader);

private:
    std::vector<n0::SceneNodePtr> m_nodes;

    std::map<int, bp::NodePtr> m_bp_nodes;

}; // ASEImporter

}
}