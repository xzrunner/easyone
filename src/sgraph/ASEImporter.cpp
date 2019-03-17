#include "sgraph/ASEImporter.h"

#include <blueprint/typedef.h>
#include <blueprint/CompNode.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>
#include <blueprint/node/Commentary.h>
#include <blueprint/node/SetLocalVar.h>
#include <blueprint/node/GetLocalVar.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/Switch.h>
#include <shadergraph/RegistNodes.h>
#include <shadergraph/PinsType.h>
#include <shadergraph/ReflectPropTypes.h>

#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <aseimp/FileLoader.h>
#include <aseimp/typedef.h>

namespace
{

bool CheckBool(const aseimp::FileLoader::Node& node, const std::string& name)
{
    auto itr = node.vars.find(name);
    assert(itr != node.vars.end() && itr->second.type == aseimp::VarType::Bool);
    return itr->second.b;
}

int CheckInt(const aseimp::FileLoader::Node& node, const std::string& name)
{
    auto itr = node.vars.find(name);
    assert(itr != node.vars.end() && itr->second.type == aseimp::VarType::Int);
    return itr->second.i;
}

float CheckFloat(const aseimp::FileLoader::Node& node, const std::string& name)
{
    auto itr = node.vars.find(name);
    assert(itr != node.vars.end() && itr->second.type == aseimp::VarType::Float);
    return itr->second.f;
}

const char* CheckString(const aseimp::FileLoader::Node& node, const std::string& name)
{
    auto itr = node.vars.find(name);
    assert(itr != node.vars.end() && itr->second.type == aseimp::VarType::String);
    return itr->second.c;
}

int port_type_aseimp_to_bp(aseimp::WirePortDataType type)
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
    Load(loader);
}

void ASEImporter::LoadShader(const std::string& filepath)
{
    aseimp::FileLoader loader;
    loader.LoadShader(filepath);
    Load(loader);
}

void ASEImporter::Load(const aseimp::FileLoader& loader)
{
    for (auto& src : loader.GetNodes())
    {
        auto dst = std::make_shared<n0::SceneNode>();
        dst->AddUniqueComp<n0::CompIdentity>();

        bp::NodePtr bp_node = nullptr;

        switch (src.cls)
        {
        case aseimp::NodeClass::Saturate:
            bp_node = std::make_shared<sg::node::Saturate>();
            break;
        case aseimp::NodeClass::Add:
            bp_node = std::make_shared<sg::node::Add>();
            break;
        case aseimp::NodeClass::Subtract:
            bp_node = std::make_shared<sg::node::Subtract>();
            break;
        case aseimp::NodeClass::Multiply:
            bp_node = std::make_shared<sg::node::Multiply>();
            break;
        case aseimp::NodeClass::Divide:
            bp_node = std::make_shared<sg::node::Divide>();
            break;
        case aseimp::NodeClass::Input:
        {
            auto input = std::make_shared<bp::node::Input>();

            input->SetName(CheckString(src, "name"));

            assert(src.outputs.size() == 1);
            input->SetType(port_type_aseimp_to_bp(src.outputs[0].data_type));

            bp_node = input;
        }
            break;
        case aseimp::NodeClass::Output:
        {
            auto output = std::make_shared<bp::node::Output>();

            output->SetName(CheckString(src, "name"));

            assert(src.inputs.size() == 1);
            output->SetType(port_type_aseimp_to_bp(src.inputs[0].data_type));

            bp_node = output;
        }
            break;
        case aseimp::NodeClass::Float:
        {
            auto vec1 = std::make_shared<sg::node::Vector1>();
            vec1->SetName(CheckString(src, "name"));
            vec1->SetValue(CheckFloat(src, "x"));
            bp_node = vec1;
        }
            break;
        case aseimp::NodeClass::Vector2:
        {
            auto vec2 = std::make_shared<sg::node::Vector2>();
            vec2->SetName(CheckString(src, "name"));
            vec2->SetValue(sm::vec2(
                CheckFloat(src, "x"),
                CheckFloat(src, "y")
            ));
            bp_node = vec2;
        }
            break;
        case aseimp::NodeClass::Vector3:
        {
            auto vec3 = std::make_shared<sg::node::Vector3>();
            vec3->SetName(CheckString(src, "name"));
            vec3->SetValue(sm::vec3(
                CheckFloat(src, "x"),
                CheckFloat(src, "y"),
                CheckFloat(src, "z")
            ));
            bp_node = vec3;
        }
            break;
        case aseimp::NodeClass::Vector4:
        {
            auto vec4 = std::make_shared<sg::node::Vector4>();
            vec4->SetName(CheckString(src, "name"));
            vec4->SetValue(sm::vec4(
                CheckFloat(src, "x"),
                CheckFloat(src, "y"),
                CheckFloat(src, "z"),
                CheckFloat(src, "w")
            ));
            bp_node = vec4;
        }
            break;
        case aseimp::NodeClass::HSVToRGB:
            bp_node = std::make_shared<sg::node::HSVToRGB>();
            break;
        case aseimp::NodeClass::RGBToHSV:
            bp_node = std::make_shared<sg::node::RGBToHSV>();
            break;
        case aseimp::NodeClass::ViewDirection:
        {
            auto view_dir = std::make_shared<sg::node::ViewDirection>();
            switch (CheckInt(src, "view_dir_space"))
            {
            case aseimp::ViewSpace::Tangent:
                view_dir->SetViewSpace(sg::ViewSpace::Tangent);
                break;
            case aseimp::ViewSpace::World:
                view_dir->SetViewSpace(sg::ViewSpace::World);
                break;
            }
            view_dir->SetSafeNormalize(CheckBool(src, "safe_normalize"));
            bp_node = view_dir;
        }
            break;
        case aseimp::NodeClass::WorldSpaceLightDir:
            bp_node = std::make_shared<sg::node::WorldSpaceLightDir>();
            break;
        case aseimp::NodeClass::Normalize:
            bp_node = std::make_shared<sg::node::Normalize>();
            break;
        case aseimp::NodeClass::Remap:
            bp_node = std::make_shared<sg::node::Remap>();
            break;
        case aseimp::NodeClass::SampleTex2D:
            bp_node = std::make_shared<sg::node::SampleTex2D>();
            break;
        case aseimp::NodeClass::ChannelMask:
        {
            auto cm = std::make_shared<sg::node::ChannelMask>();
            sg::PropMultiChannels c = 0;
            if (CheckBool(src, "r")) {
                c.channels |= sg::PropMultiChannels::CHANNEL_R;
            }
            if (CheckBool(src, "g")) {
                c.channels |= sg::PropMultiChannels::CHANNEL_G;
            }
            if (CheckBool(src, "b")) {
                c.channels |= sg::PropMultiChannels::CHANNEL_B;
            }
            if (CheckBool(src, "a")) {
                c.channels |= sg::PropMultiChannels::CHANNEL_A;
            }
            bp_node = cm;
        }
            break;
        case aseimp::NodeClass::ViewMatrix:
            bp_node = std::make_shared<sg::node::ViewMatrix>();
            break;
        case aseimp::NodeClass::Commentary:
        {
            auto comm = std::make_shared<bp::node::Commentary>();
            comm->SetWidth(CheckFloat(src, "width"));
            comm->SetHeight(CheckFloat(src, "height"));
            comm->SetCommentText(CheckString(src, "text"));
            bp_node = comm;
        }
            break;
        case aseimp::NodeClass::WorldNormalVector:
            bp_node = std::make_shared<sg::node::WorldNormalVector>();
            break;
        case aseimp::NodeClass::SetLocalVar:
        {
            auto set_var = std::make_shared<bp::node::SetLocalVar>();
            set_var->SetVarName(CheckString(src, "name"));
            bp_node = set_var;
        }
            break;
        case aseimp::NodeClass::GetLocalVar:
            bp_node = std::make_shared<bp::node::GetLocalVar>();
            break;
        case aseimp::NodeClass::Function:
        {
            auto func = std::make_shared<bp::node::Function>();

            std::string filepath = CheckString(src, "filename");
            func->SetFilepath(filepath);

            bp_node = func;
        }
            break;
        case aseimp::NodeClass::DotProduct:
            bp_node = std::make_shared<sg::node::DotProduct>();
            break;
        case aseimp::NodeClass::Tex2DAsset:
            bp_node = std::make_shared<sg::node::Tex2DAsset>();
            break;
        case aseimp::NodeClass::Exponential:
        {
            auto exp = std::make_shared<sg::node::Exponential>();
            exp->SetType(sg::PropMathBaseType::BASE_2);
            bp_node = exp;
        }
            break;
        case aseimp::NodeClass::TexCoords:
            bp_node = std::make_shared<sg::node::TexCoords>();
            break;
        case aseimp::NodeClass::TextureTransform:
            bp_node = std::make_shared<sg::node::TextureTransform>();
            break;
        case aseimp::NodeClass::Power:
            bp_node = std::make_shared<sg::node::Power>();
            break;
        case aseimp::NodeClass::ScaleAndOffset:
            bp_node = std::make_shared<sg::node::ScaleAndOffset>();
            break;
        case aseimp::NodeClass::IndirectDiffuseLighting:
            bp_node = std::make_shared<sg::node::IndirectDiffuseLighting>();
            break;
        case aseimp::NodeClass::IndirectSpecularLight:
            bp_node = std::make_shared<sg::node::IndirectSpecularLight>();
            break;
        case aseimp::NodeClass::Combine:
            bp_node = std::make_shared<sg::node::Combine>();
            break;
        case aseimp::NodeClass::LightColor:
            bp_node = std::make_shared<sg::node::LightColor>();
            break;
        case aseimp::NodeClass::Lerp:
            bp_node = std::make_shared<sg::node::Lerp>();
            break;
        case aseimp::NodeClass::Switch:
            bp_node = std::make_shared<bp::node::Switch>();
            break;
        case aseimp::NodeClass::Smoothstep:
            bp_node = std::make_shared<sg::node::Smoothstep>();
            break;
        case aseimp::NodeClass::StandardSurfaceOutput:
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

        m_nodes.push_back(dst);
        m_bp_nodes.insert({ src.uid, bp_node });
    }

    for (auto& conn : loader.GetConns())
    {
        int node_to   = conn.node_to;
        int port_to   = conn.port_to;
        int node_from = conn.node_from;
        int port_from = conn.port_from;

        auto from = m_bp_nodes.find(node_from);
        auto to   = m_bp_nodes.find(node_to);
        if (from == m_bp_nodes.end() ||
            to == m_bp_nodes.end()) {
            return;
        }

        auto from_type = from->second->get_type();
        if (from_type == rttr::type::get<sg::node::Vector2>() ||
            from_type == rttr::type::get<sg::node::Vector3>() ||
            from_type == rttr::type::get<sg::node::Vector4>())
        {
            assert(port_to == 0);
        }

        auto to_type = to->second->get_type();
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

        auto& from_ports = from->second->GetAllOutput();
        auto& to_ports   = to->second->GetAllInput();
        if (port_from >= 0 && port_from < from_ports.size() &&
            port_to >= 0 && port_to < to_ports.size()) {
            bp::make_connecting(from_ports[port_from], to_ports[port_to]);
        } else {
            if (port_from < 0 || port_from >= from_ports.size()) {
                assert(from->second->get_type() == rttr::type::get<bp::node::Function>());
            }
            if (port_to < 0 || port_to >= to_ports.size()) {
                assert(to->second->get_type() == rttr::type::get<bp::node::Function>() ||
                    to->second->get_type() == rttr::type::get<sg::node::Multiply>());
            }
        }
    }
}

}
}