#include "sgraph/ASEImporter.h"

#include <blueprint/typedef.h>
#include <blueprint/CompNode.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>
#include <blueprint/node/Commentary.h>
#include <blueprint/node/SetReference.h>
#include <blueprint/node/GetReference.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Switch.h>
#include <shadergraph/RegistNodes.h>
#include <shadergraph/PinsType.h>
#include <shadergraph/ReflectPropTypes.h>
#include <shadergraph/node/Custom.h>
#include <shadergraph/node/StandardSurfaceOutput.h>

#include <cpputil/StringHelper.h>
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <aseimp/FileLoader.h>
#include <aseimp/typedef.h>

#include <boost/filesystem.hpp>

namespace
{

bool CheckBool(const aseimp::FileLoader::Node& node, const std::string& name)
{
    auto itr = node.vars.find(name);
    if (itr == node.vars.end() || itr->second.type != aseimp::VarType::Bool) {
        throw std::exception("check fail.");
    }
    return itr->second.b;
}

int CheckInt(const aseimp::FileLoader::Node& node, const std::string& name)
{
    auto itr = node.vars.find(name);
    if (itr == node.vars.end() || itr->second.type != aseimp::VarType::Int) {
        throw std::exception("check fail.");
    }
    return itr->second.i;
}

float CheckFloat(const aseimp::FileLoader::Node& node, const std::string& name)
{
    auto itr = node.vars.find(name);
    if (itr == node.vars.end() || itr->second.type != aseimp::VarType::Float) {
        throw std::exception("check fail.");
    }
    return itr->second.f;
}

const char* CheckString(const aseimp::FileLoader::Node& node, const std::string& name)
{
    auto itr = node.vars.find(name);
    if (itr == node.vars.end() || itr->second.type != aseimp::VarType::String) {
        throw std::exception("check fail.");
    }
    return itr->second.c;
}

bool QueryBool(const aseimp::FileLoader::Node& node, const std::string& name, bool& ret)
{
    auto itr = node.vars.find(name);
    if (itr == node.vars.end()) {
        return false;
    }

    assert(itr->second.type == aseimp::VarType::Bool);
    ret = itr->second.b;
    return true;
}

bool QueryInt(const aseimp::FileLoader::Node& node, const std::string& name, int& ret)
{
    auto itr = node.vars.find(name);
    if (itr == node.vars.end()) {
        return false;
    }

    assert(itr->second.type == aseimp::VarType::Int);
    ret = itr->second.i;
    return true;
}

bool QueryFloat(const aseimp::FileLoader::Node& node, const std::string& name, float& ret)
{
    auto itr = node.vars.find(name);
    if (itr == node.vars.end()) {
        return false;
    }

    assert(itr->second.type == aseimp::VarType::Float);
    ret = itr->second.f;
    return true;
}

bool QueryString(const aseimp::FileLoader::Node& node, const std::string& name, std::string& ret)
{
    auto itr = node.vars.find(name);
    if (itr == node.vars.end()) {
        return false;
    }

    assert(itr->second.type == aseimp::VarType::String);
    ret = itr->second.c;
    return true;
}

}

namespace eone
{
namespace sgraph
{

ASEImporter::ASEImporter()
{
    aseimp::regist_rttr_typedef();
}

void ASEImporter::LoadAsset(const std::string& filepath)
{
    aseimp::FileLoader loader;
    loader.LoadAsset(filepath);

    auto dir = boost::filesystem::path(filepath).parent_path().string();
    Load(loader, dir);
}

void ASEImporter::LoadShader(const std::string& filepath)
{
    aseimp::FileLoader loader;
    loader.LoadShader(filepath);

    auto dir = boost::filesystem::path(filepath).parent_path().string();
    Load(loader, dir);
}

void ASEImporter::Load(const aseimp::FileLoader& loader, const std::string& dir)
{
    for (auto& src : loader.GetNodes())
    {
        auto dst = std::make_shared<n0::SceneNode>();
        dst->AddUniqueComp<n0::CompIdentity>();

        bp::NodePtr bp_node = nullptr;

        switch (src.cls)
        {
            // Math Operators
        case aseimp::NodeClass::Saturate:
            bp_node = std::make_shared<sg::node::Saturate>();
            break;
        case aseimp::NodeClass::Add:
            bp_node = std::make_shared<sg::node::Add>();
            bp_node->PrepareExtInputPorts(src.inputs.size());
            break;
        case aseimp::NodeClass::Subtract:
            bp_node = std::make_shared<sg::node::Subtract>();
            break;
        case aseimp::NodeClass::Multiply:
            bp_node = std::make_shared<sg::node::Multiply>();
            bp_node->PrepareExtInputPorts(src.inputs.size());
            break;
        case aseimp::NodeClass::Divide:
            bp_node = std::make_shared<sg::node::Divide>();
            break;
        case aseimp::NodeClass::Remap:
            bp_node = std::make_shared<sg::node::Remap>();
            break;
        case aseimp::NodeClass::Exponential:
        {
            auto exp = std::make_shared<sg::node::Exponential>();
            exp->SetType(sg::PropMathBaseType::BASE_2);
            bp_node = exp;
        }
            break;
        case aseimp::NodeClass::Power:
            bp_node = std::make_shared<sg::node::Power>();
            break;
        case aseimp::NodeClass::ScaleAndOffset:
            bp_node = std::make_shared<sg::node::ScaleAndOffset>();
            break;
        case aseimp::NodeClass::Lerp:
            bp_node = std::make_shared<sg::node::Lerp>();
            break;
        case aseimp::NodeClass::Smoothstep:
            bp_node = std::make_shared<sg::node::Smoothstep>();
            break;

            // Logical Operators
        case aseimp::NodeClass::Switch:
            bp_node = std::make_shared<bp::node::Switch>();
            break;

            // Functions
        case aseimp::NodeClass::Input:
        {
            auto input = std::make_shared<bp::node::Input>();

            std::string name;
            if (QueryString(src, "name", name)) {
                input->SetName(name);
            }

            assert(src.outputs.size() == 1);
            input->SetType(PortTypeASEImpToBP(src.outputs[0].data_type));

            bp_node = input;
        }
            break;
        case aseimp::NodeClass::Output:
        {
            auto output = std::make_shared<bp::node::Output>();

            std::string name;
            if (QueryString(src, "name", name)) {
                output->SetName(name);
            }

            assert(src.inputs.size() == 1);
            output->SetType(PortTypeASEImpToBP(src.inputs[0].data_type));

            bp_node = output;
        }
            break;
        case aseimp::NodeClass::Function:
        {
            auto func = std::make_shared<bp::node::Function>();

            std::string filename;
            if (QueryString(src, "filename", filename))
            {
                func->SetFilepath(filename);

                auto path = boost::filesystem::absolute(filename + ".asset", dir);
                if (boost::filesystem::exists(path))
                {
                    ASEImporter loader;
                    loader.LoadAsset(path.string());
                    func->SetChildren(func, loader.GetNodes());
                }
            }

            bp_node = func;
        }
            break;

            // Constants And Properties
        case aseimp::NodeClass::Float:
        {
            auto vec1 = std::make_shared<sg::node::Vector1>();

            std::string name;
            if (QueryString(src, "name", name)) {
                vec1->SetName(name);
            }
            float x;
            if (QueryFloat(src, "x", x)) {
                vec1->SetValue(x);
            }

            bp_node = vec1;
        }
            break;
        case aseimp::NodeClass::Vector2:
        {
            auto vec2 = std::make_shared<sg::node::Vector2>();

            std::string name;
            if (QueryString(src, "name", name)) {
                vec2->SetName(name);
            }
            float x, y;
            if (QueryFloat(src, "x", x) &&
                QueryFloat(src, "y", y)) {
                vec2->SetValue(sm::vec2(x, y));
            }

            bp_node = vec2;
        }
            break;
        case aseimp::NodeClass::Vector3:
        {
            auto vec3 = std::make_shared<sg::node::Vector3>();

            std::string name;
            if (QueryString(src, "name", name)) {
                vec3->SetName(name);
            }
            float x, y, z;
            if (QueryFloat(src, "x", x) &&
                QueryFloat(src, "y", y) &&
                QueryFloat(src, "z", z)) {
                vec3->SetValue(sm::vec3(x, y, z));
            }

            bp_node = vec3;
        }
            break;
        case aseimp::NodeClass::Vector4:
        {
            auto vec4 = std::make_shared<sg::node::Vector4>();

            std::string name;
            if (QueryString(src, "name", name)) {
                vec4->SetName(name);
            }
            float x, y, z, w;
            if (QueryFloat(src, "x", x) &&
                QueryFloat(src, "y", y) &&
                QueryFloat(src, "z", z) &&
                QueryFloat(src, "w", w)) {
                vec4->SetValue(sm::vec4(x, y, z, w));
            }

            bp_node = vec4;
        }
            break;

            // Image Effects
        case aseimp::NodeClass::HSVToRGB:
            bp_node = std::make_shared<sg::node::HSVToRGB>();
            break;
        case aseimp::NodeClass::RGBToHSV:
            bp_node = std::make_shared<sg::node::RGBToHSV>();
            break;

            // Camera And Screen
        case aseimp::NodeClass::ViewDirection:
        {
            auto view_dir = std::make_shared<sg::node::ViewDirection>();

            int view_dir_space;
            if (QueryInt(src, "view_dir_space", view_dir_space))
            {
                switch (view_dir_space)
                {
                case aseimp::ViewSpace::Tangent:
                    view_dir->SetViewSpace(sg::ViewSpace::Tangent);
                    break;
                case aseimp::ViewSpace::World:
                    view_dir->SetViewSpace(sg::ViewSpace::World);
                    break;
                }
            }

            bool safe_normalize;
            if (QueryBool(src, "safe_normalize", safe_normalize)) {
                view_dir->SetSafeNormalize(safe_normalize);
            }

            bp_node = view_dir;
        }
            break;

            // Light
        case aseimp::NodeClass::WorldSpaceLightDir:
            bp_node = std::make_shared<sg::node::WorldSpaceLightDir>();
            break;
        case aseimp::NodeClass::LightColor:
            bp_node = std::make_shared<sg::node::LightColor>();
            break;
        case aseimp::NodeClass::IndirectDiffuseLighting:
            bp_node = std::make_shared<sg::node::IndirectDiffuseLighting>();
            break;
        case aseimp::NodeClass::IndirectSpecularLight:
            bp_node = std::make_shared<sg::node::IndirectSpecularLight>();
            break;

            // Vector Operators
        case aseimp::NodeClass::Normalize:
            bp_node = std::make_shared<sg::node::Normalize>();
            break;
        case aseimp::NodeClass::ChannelMask:
        {
            auto cm = std::make_shared<sg::node::ChannelMask>();

            bool r, g, b, a;
            if (QueryBool(src, "r", r) &&
                QueryBool(src, "g", g) &&
                QueryBool(src, "b", b) &&
                QueryBool(src, "a", a))
            {
                sg::PropMultiChannels c = 0;
                if (r) {
                    c.channels |= sg::PropMultiChannels::CHANNEL_R;
                }
                if (g) {
                    c.channels |= sg::PropMultiChannels::CHANNEL_G;
                }
                if (b) {
                    c.channels |= sg::PropMultiChannels::CHANNEL_B;
                }
                if (a) {
                    c.channels |= sg::PropMultiChannels::CHANNEL_A;
                }
                cm->SetChannels(c);
            }

            bp_node = cm;
        }
            break;
        case aseimp::NodeClass::DotProduct:
            bp_node = std::make_shared<sg::node::DotProduct>();
            break;
        case aseimp::NodeClass::Combine:
            bp_node = std::make_shared<sg::node::Combine>();
            break;

            // Matrix Transform
        case aseimp::NodeClass::ViewMatrix:
            bp_node = std::make_shared<sg::node::ViewMatrix>();
            break;

            // UV Coordinates
        case aseimp::NodeClass::TexCoords:
            bp_node = std::make_shared<sg::node::TexCoords>();
            break;

            // Textures
        case aseimp::NodeClass::Tex2DAsset:
            bp_node = std::make_shared<sg::node::Tex2DAsset>();
            break;
        case aseimp::NodeClass::SampleTex2D:
            bp_node = std::make_shared<sg::node::SampleTex2D>();
            break;
        case aseimp::NodeClass::TextureTransform:
            bp_node = std::make_shared<sg::node::TextureTransform>();
            break;

            // Surface Data
        case aseimp::NodeClass::WorldNormalVector:
            bp_node = std::make_shared<sg::node::WorldNormalVector>();
            break;

            // Miscellaneous
        case aseimp::NodeClass::SetReference:
        {
            auto set_var = std::make_shared<bp::node::SetReference>();

            std::string name;
            if (QueryString(src, "name", name)) {
                set_var->SetName(name);
            }

            bp_node = set_var;
        }
            break;
        case aseimp::NodeClass::GetReference:
            bp_node = std::make_shared<bp::node::GetReference>();
            break;
        case aseimp::NodeClass::CustomExpression:
        {
            auto custom = std::make_shared<sg::node::Custom>();

            std::vector<sg::Node::PinsDesc> inputs, outputs;
            InputPortASEImpToSG(inputs, src.inputs);
            OutputPortASEImpToSG(outputs, src.outputs);

            int input_count = CheckInt(src, "input_count");
            assert(input_count == inputs.size());
            for (int i = 0; i < input_count; ++i) {
                inputs[i].name = CheckString(src, "input" + std::to_string(i));
            }

            custom->ResetPorts(inputs, outputs);

            bp_node = custom;
        }
            break;

            // Tools
        case aseimp::NodeClass::Commentary:
        {
            auto comm = std::make_shared<bp::node::Commentary>();

            float width;
            if (QueryFloat(src, "width", width)) {
                comm->SetWidth(width);
            }
            float height;
            if (QueryFloat(src, "height", height)) {
                comm->SetHeight(height);
            }

            std::string text;
            if (QueryString(src, "text", text)) {
                text = cpputil::StringHelper::UTF8ToGBK(text.c_str());
                comm->SetCommentText(text);
            }

            bp_node = comm;
        }
            break;

            // Master
        case aseimp::NodeClass::StandardSurfaceOutput:
        {
            int light_model;
            if (QueryInt(src, "light_model", light_model)) {
                assert(light_model == static_cast<int>(aseimp::StandardShaderLightModel::CustomLighting));
            }
            bp_node = std::make_shared<sg::node::StandardSurfaceOutput>();
        }
            break;
        }

        if (!bp_node) {
            continue;
        }

        auto& cnode = dst->AddUniqueComp<bp::CompNode>();
        cnode.SetNode(bp_node);

        auto& style = bp_node->GetStyle();
        dst->AddUniqueComp<n2::CompBoundingBox>(
            sm::rect(style.width, style.height)
        );

        auto& ctrans = dst->AddUniqueComp<n2::CompTransform>();
        const float x = src.x + style.width * 0.5f;
        const float y = -(src.y + style.height * 0.5f);
        ctrans.SetPosition(*dst, sm::vec2(x, y));

        m_scene_nodes.push_back(dst);
        m_map_nodes.insert({ src.uid, { src, bp_node } });
    }

    for (auto& conn : loader.GetConns())
    {
        int node_to   = conn.node_to;
        int port_to   = conn.port_to;
        int node_from = conn.node_from;
        int port_from = conn.port_from;

        auto from = m_map_nodes.find(node_from);
        auto to   = m_map_nodes.find(node_to);
        if (from == m_map_nodes.end() ||
            to == m_map_nodes.end()) {
            return;
        }

        auto from_type = from->second.dst->get_type();
        if (from_type == rttr::type::get<sg::node::Vector2>() ||
            from_type == rttr::type::get<sg::node::Vector3>() ||
            from_type == rttr::type::get<sg::node::Vector4>())
        {
            assert(port_to == 0);
        }

        auto to_type = to->second.dst->get_type();
        if (to_type == rttr::type::get<sg::node::Smoothstep>())
        {
            switch (port_to)
            {
            case 0:
                port_to = 2;
                break;
            case 1:
                port_to = 0;
                break;
            case 2:
                port_to = 1;
                break;
            }
        }

        int idx_port_from = -1, idx_port_to = -1;
        for (int i = 0, n = from->second.src.outputs.size(); i < n; ++i) {
            if (port_from == from->second.src.outputs[i].port_id) {
                idx_port_from = i;
                break;
            }
        }
        for (int i = 0, n = to->second.src.inputs.size(); i < n; ++i) {
            if (port_to == to->second.src.inputs[i].port_id) {
                idx_port_to = i;
                break;
            }
        }
        if (idx_port_from == -1) {
            idx_port_from = port_from;
        }
        if (idx_port_to == -1) {
            idx_port_to = port_to;
        }

        auto& from_ports = from->second.dst->GetAllOutput();
        auto& to_ports   = to->second.dst->GetAllInput();
        if (idx_port_from >= 0 && idx_port_from < static_cast<int>(from_ports.size()) &&
            idx_port_to >= 0 && idx_port_to < static_cast<int>(to_ports.size())) {
            bp::make_connecting(from_ports[idx_port_from], to_ports[idx_port_to]);
        } else {
            printf("conn fail:%d %d to %d %d\n", node_from, port_from, node_to, port_to);
        }
    }
}

int ASEImporter::PortTypeASEImpToBP(aseimp::WirePortDataType type)
{
    int ret = bp::PINS_ANY_VAR;
    switch (type)
    {
    case aseimp::WirePortDataType::FLOAT:
        ret = sg::PINS_VECTOR1;
        break;
    case aseimp::WirePortDataType::FLOAT2:
        ret = sg::PINS_VECTOR2;
        break;
    case aseimp::WirePortDataType::FLOAT3:
        ret = sg::PINS_VECTOR3;
        break;
    case aseimp::WirePortDataType::FLOAT4:
        ret = sg::PINS_VECTOR4;
        break;
    case aseimp::WirePortDataType::COLOR:
        ret = sg::PINS_COLOR;
        break;
    case aseimp::WirePortDataType::SAMPLER2D:
        ret = sg::PINS_TEXTURE2D;
        break;
    case aseimp::WirePortDataType::FLOAT3x3:
        ret = sg::PINS_MATRIX3;
        break;
    case aseimp::WirePortDataType::FLOAT4x4:
        ret = sg::PINS_MATRIX4;
        break;
    case aseimp::WirePortDataType::OBJECT:
        ret = bp::PINS_ANY_VAR;
        break;
    default:
        assert(0);
    }
    return ret;
}

void ASEImporter::InputPortASEImpToSG(std::vector<sg::Node::PinsDesc>& dst,
                                      const std::vector<aseimp::FileLoader::InputPort>& src)
{
    dst.resize(src.size());
    for (int i = 0, n = src.size(); i < n; ++i)
    {
        auto& s = src[i];
        auto& d = dst[i];
        d.type = PortTypeASEImpToBP(s.data_type);
        d.name = s.name;
    }
}

void ASEImporter::OutputPortASEImpToSG(std::vector<sg::Node::PinsDesc>& dst,
                                       const std::vector<aseimp::FileLoader::OutputPort>& src)
{
    dst.resize(src.size());
    for (int i = 0, n = src.size(); i < n; ++i)
    {
        auto& s = src[i];
        auto& d = dst[i];
        d.type = PortTypeASEImpToBP(s.data_type);
        d.name = "out";
    }
}

}
}