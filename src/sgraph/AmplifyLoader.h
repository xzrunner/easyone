#pragma once

#include <blueprint/typedef.h>

#include <node0/typedef.h>

#include <string>
#include <vector>
#include <map>

namespace eone
{
namespace sgraph
{

class AmplifyLoader
{
public:
    AmplifyLoader() {}

    void LoadAsset(const std::string& filepath);

    auto& GetNodes() const { return m_nodes; }

private:
    void LoadASE(const std::string& str);

    void LoadNode(const std::vector<std::string>& tokens);
    void LoadConn(const std::vector<std::string>& tokens);

    static bool ToBool(const std::string& str);
    static int ToDataType(const std::string& str);

private:
    std::vector<n0::SceneNodePtr> m_nodes;

    std::map<int, bp::NodePtr> m_bp_nodes;

}; // AmplifyLoader
   
}
}
