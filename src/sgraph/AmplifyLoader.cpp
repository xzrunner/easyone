#include "sgraph/AmplifyLoader.h"

#include <blueprint/typedef.h>
#include <blueprint/CompNode.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>
#include <shadergraph/RegistNodes.h>
#include <shadergraph/PinsType.h>

#include <cpputil/StringHelper.h>
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>

#include <yaml-cpp/yaml.h>

#include <assert.h>

namespace
{

struct InputPort
{
    int         port_id = -1;
    int         data_type = -1;
    std::string internal_data;
    bool        editable = false;
    std::string name;
};

struct OutputPort
{
    int data_type = -1;
    int port_id   = -1;
};

}

namespace eone
{
namespace sgraph
{

void AmplifyLoader::LoadAsset(const std::string& filepath)
{
    auto root = YAML::LoadFile(filepath);

    auto mono = root["MonoBehaviour"];
    assert(!mono.IsNull());

    auto name = mono["m_Name"].as<std::string>();
    auto func = mono["m_functionInfo"].as<std::string>();

    LoadASE(func);
}

void AmplifyLoader::LoadASE(const std::string& str)
{
    std::stringstream ss(str);
    std::string line;

    while (std::getline(ss, line))
    {
        if (line.find("ASEBEGIN") != std::string::npos) {
            break;
        }
    }

    // version
    std::getline(ss, line);

    // camera
    // x, y, w, h, offx, offy, zoom, NodeParametersWindowMaximized, PaletteWindowMaximized
    std::getline(ss, line);
 
    while (std::getline(ss, line))
    {
        if (line.find("ASEEND") != std::string::npos) {
            break;
        }

        std::vector<std::string> tokens;
        cpputil::StringHelper::Split(line, ";", tokens);
        assert(!tokens.empty());

        if (tokens[0] == "Node") {
            LoadNode(tokens);
        } else if (tokens[0] == "WireConnection") {
            LoadConn(tokens);
        }
    }
}

void AmplifyLoader::LoadNode(const std::vector<std::string>& tokens)
{
    int ptr = 0;
    ++ptr;  // skip Node
    ++ptr;  // skip name

    auto node = std::make_shared<n0::SceneNode>();
    node->AddUniqueComp<n0::CompIdentity>();

    bp::NodePtr bp_node = nullptr;

    int uid = std::atoi(tokens[ptr++].c_str());

    sm::vec2 pos;
    std::vector<std::string> pos_tokens;
    cpputil::StringHelper::Split(tokens[ptr++], ",", pos_tokens);
    assert(pos_tokens.size() == 2);
    pos.x = static_cast<float>(std::atof(pos_tokens[0].c_str()));
    pos.y = static_cast<float>(std::atof(pos_tokens[1].c_str()));
    auto& ctrans = node->AddUniqueComp<n2::CompTransform>();
    ctrans.SetPosition(*node, pos);

    auto precision_type = tokens[ptr++];

    bool preview = ToBool(tokens[ptr++]);

    assert(tokens.size() >= 2);
    std::string name = tokens[1].substr(strlen("AmplifyShaderEditor."));
    if (name == "SaturateNode") 
    {
        bp_node = std::make_shared<sg::node::Saturate>();
    }
    else if (name == "SimpleAddOpNode")
    {
        int input_count = std::atoi(tokens[ptr++].c_str());

        bp_node = std::make_shared<sg::node::Add>();
    }
    else if (name == "FunctionInput")
    {
        auto input_name = tokens[ptr++];
        int selected_input_type_int = std::atoi(tokens[ptr++].c_str());
        int order_index = std::atoi(tokens[ptr++].c_str());
        bool auto_cast = ToBool(tokens[ptr++]);

        bp_node = std::make_shared<bp::node::Input>();
    }
    else if (name == "FunctionOutput")
    {
        // todo

        bp_node = std::make_shared<bp::node::Output>();
    }
    else if (name == "RangedFloatNode")
    {
        // todo

        bp_node = std::make_shared<sg::node::Vector1>();
    }

    if (!bp_node) {
        return;
    }

    std::vector<InputPort> inputs;
    int input_count = std::atoi(tokens[ptr++].c_str());
    inputs.resize(input_count);
    for (int i = 0; i < input_count; ++i)
    {
        auto& dst = inputs[i];
        dst.port_id = std::atoi(tokens[ptr++].c_str());
        dst.data_type = ToDataType(tokens[ptr++]);
        dst.internal_data = tokens[ptr++];
        dst.editable = ToBool(tokens[ptr++]);
        if (dst.editable && ptr < static_cast<int>(tokens.size())) {
            dst.name = tokens[ptr++];
        }
    }

    std::vector<OutputPort> outputs;
    int output_count = std::atoi(tokens[ptr++].c_str());
    outputs.resize(output_count);
    for (int i = 0; i < output_count; ++i)
    {
        auto& dst = outputs[i];
        dst.data_type = ToDataType(tokens[ptr++]);
        dst.port_id = std::atoi(tokens[ptr++].c_str());
    }

    auto& cnode = node->AddUniqueComp<bp::CompNode>();
    cnode.SetNode(bp_node);

    auto& style = bp_node->GetStyle();
	node->AddUniqueComp<n2::CompBoundingBox>(
		sm::rect(style.width, style.height)
	);

    m_nodes.push_back(node);
    m_bp_nodes.insert({ uid, bp_node });
}

void AmplifyLoader::LoadConn(const std::vector<std::string>& tokens)
{
    int ptr = 0;
    ++ptr;  // skip WireConnection

    int node_to   = std::atoi(tokens[ptr++].c_str());
    int port_to   = std::atoi(tokens[ptr++].c_str());
    int node_from = std::atoi(tokens[ptr++].c_str());
    int port_from = std::atoi(tokens[ptr++].c_str());

    auto from = m_bp_nodes.find(node_from);
    auto to   = m_bp_nodes.find(node_to);
    if (from == m_bp_nodes.end() ||
        to == m_bp_nodes.end()) {
        return;
    }

    bp::make_connecting(
        from->second->GetAllOutput()[port_from],
        to->second->GetAllInput()[port_to]
    );
}

bool AmplifyLoader::ToBool(const std::string& str)
{
    if (str == "True") {
        return true;
    } else if (str == "False") {
        return false;
    } else {
        assert(0);
        return true;
    }
}

int AmplifyLoader::ToDataType(const std::string& str)
{
    int type = -1;

    if (str == "FLOAT") {
        type = sg::PINS_VECTOR1;
    } else if (str == "FLOAT2") {
        type = sg::PINS_VECTOR2;
    } else if (str == "FLOAT3") {
        type = sg::PINS_VECTOR3;
    } else if (str == "FLOAT4") {
        type = sg::PINS_VECTOR4;
    } else if (str == "COLOR") {
        type = sg::PINS_COLOR;
    } else if (str == "SAMPLER2D") {
        type = sg::PINS_TEXTURE2D;
    } else if (str == "FLOAT3x3") {
        type = sg::PINS_MATRIX3;
    } else if (str == "FLOAT4x4") {
        type = sg::PINS_MATRIX4;
    } else {
        assert(0);
    }

    return type;
}

}
}
