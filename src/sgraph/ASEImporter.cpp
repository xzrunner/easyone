#include "sgraph/ASEImporter.h"

#include <ee0/AssetsMap.h>
#include <blueprint/typedef.h>
#include <blueprint/CompNode.h>
#include <blueprint/Connecting.h>
#include <blueprint/node/Input.h>
#include <blueprint/node/Output.h>
#include <blueprint/node/Commentary.h>
#include <blueprint/node/SetReference.h>
#include <blueprint/node/GetReference.h>
#include <blueprint/node/Function.h>
#include <blueprint/node/CompareLess.h>
#include <blueprint/node/CompareEqual.h>
#include <blueprint/node/CompareGreater.h>
#include <blueprint/node/CompareNotEqual.h>
#include <blueprint/node/Switch.h>
#include <blueprint/node/SwitchMulti.h>
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

    Load(loader);
}

void ASEImporter::LoadShader(const std::string& filepath)
{
    auto dir = boost::filesystem::path(filepath).parent_path().string();
    ee0::AssetsMap::Instance()->LoadDirWithUnity(dir);

    aseimp::FileLoader loader;
    loader.LoadShader(filepath);

    Load(loader);
}

void ASEImporter::Load(const aseimp::FileLoader& loader)
{
    for (auto& src : loader.GetNodes())
    {
        bp::NodePtr bp_node = nullptr;

        switch (src.cls)
        {
            // Math Operators
        case aseimp::NodeClass::Absolute:
            bp_node = std::make_shared<sg::node::Absolute>();
            break;
        case aseimp::NodeClass::Saturate:
            bp_node = std::make_shared<sg::node::Saturate>();
            break;
        case aseimp::NodeClass::Minimum:
            bp_node = std::make_shared<sg::node::Minimum>();
            break;
        case aseimp::NodeClass::Maximum:
            bp_node = std::make_shared<sg::node::Maximum>();
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
        case aseimp::NodeClass::OneMinus:
            bp_node = std::make_shared<sg::node::OneMinus>();
            break;
        case aseimp::NodeClass::Remap:
            bp_node = std::make_shared<sg::node::Remap>();
            break;
        case aseimp::NodeClass::Exp2:
        {
            auto exp = std::make_shared<sg::node::Exponential>();
            exp->SetType(sg::PropMathBaseType::BASE_2);
            bp_node = exp;
        }
            break;
        case aseimp::NodeClass::ExpE:
        {
            auto exp = std::make_shared<sg::node::Exponential>();
            exp->SetType(sg::PropMathBaseType::BASE_E);
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
        case aseimp::NodeClass::FWidth:
            bp_node = std::make_shared<sg::node::FWidth>();
            break;
        case aseimp::NodeClass::Sine:
            bp_node = std::make_shared<sg::node::Sine>();
            break;
        case aseimp::NodeClass::Cosine:
            bp_node = std::make_shared<sg::node::Cosine>();
            break;
        case aseimp::NodeClass::Tangent:
            bp_node = std::make_shared<sg::node::Tangent>();
            break;
        case aseimp::NodeClass::Arcsine:
            bp_node = std::make_shared<sg::node::Arcsine>();
            break;
        case aseimp::NodeClass::Arccosine:
            bp_node = std::make_shared<sg::node::Arccosine>();
            break;

            // Logical Operators
        case aseimp::NodeClass::CompareLess:
            bp_node = std::make_shared<bp::node::CompareLess>();
            break;
        case aseimp::NodeClass::CompareEqual:
            bp_node = std::make_shared<bp::node::CompareEqual>();
            break;
        case aseimp::NodeClass::CompareGreater:
            bp_node = std::make_shared<bp::node::CompareGreater>();
            break;
        case aseimp::NodeClass::CompareNotEqual:
            bp_node = std::make_shared<bp::node::CompareNotEqual>();
            break;
        case aseimp::NodeClass::Switch:
            bp_node = std::make_shared<bp::node::Switch>();
            break;
        case aseimp::NodeClass::SwitchMulti:
            bp_node = std::make_shared<bp::node::SwitchMulti>();
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

            std::string function_guid;
            if (QueryString(src, "function_guid", function_guid))
            {
                auto filepath = ee0::AssetsMap::Instance()->QueryFilepath(function_guid);
                if (!filepath.empty()) 
                {
                    func->SetFilepath(filepath);
                    ASEImporter loader;
                    loader.LoadAsset(filepath);
                    func->SetChildren(func, loader.GetNodes());
                }
                else
                {
                    printf("unkown function_guid %s\n", function_guid.c_str());
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
        case aseimp::NodeClass::PI:
            bp_node = std::make_shared<sg::node::PI>();
            break;
        case aseimp::NodeClass::Color:
            bp_node = std::make_shared<sg::node::Color>();
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
        case aseimp::NodeClass::LightAttenuation:
            bp_node = std::make_shared<sg::node::LightAttenuation>();
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
        case aseimp::NodeClass::Split:
            bp_node = std::make_shared<sg::node::Split>();
            break;
        case aseimp::NodeClass::TransformDirection:
            bp_node = std::make_shared<sg::node::TransformDirection>();
            break;
        case aseimp::NodeClass::Length:
            bp_node = std::make_shared<sg::node::Length>();
            break;

            // Vertex Data
        case aseimp::NodeClass::VertexBitangent:
            bp_node = std::make_shared<sg::node::VertexBitangent>();
            break;
        case aseimp::NodeClass::VertexNormal:
            bp_node = std::make_shared<sg::node::VertexNormal>();
            break;
        case aseimp::NodeClass::VertexTangent:
            bp_node = std::make_shared<sg::node::VertexTangent>();
            break;

            // Surface Data
        case aseimp::NodeClass::WorldBitangent:
            bp_node = std::make_shared<sg::node::WorldBitangent>();
            break;
        case aseimp::NodeClass::WorldPosition:
            bp_node = std::make_shared<sg::node::WorldPosition>();
            break;
        case aseimp::NodeClass::WorldTangent:
            bp_node = std::make_shared<sg::node::WorldTangent>();
            break;

            // Matrix Operators
        case aseimp::NodeClass::MatrixConstruction:
            bp_node = std::make_shared<sg::node::MatrixConstruction>();
            break;
        case aseimp::NodeClass::MatrixInverse:
            bp_node = std::make_shared<sg::node::MatrixInverse>();
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
        {
            auto tex = std::make_shared<sg::node::Tex2DAsset>();

            std::string tex_guid;
            if (QueryString(src, "material_tex_guid", tex_guid)) {
                auto filepath = ee0::AssetsMap::Instance()->QueryFilepath(tex_guid);
                if (!filepath.empty()) {
                    tex->SetImagePath(filepath);
                }
            }

            bp_node = tex;
        }
            break;
        case aseimp::NodeClass::SampleTex2D:
        {
            bp_node = std::make_shared<sg::node::SampleTex2D>();

            std::string tex_guid;
            if (QueryString(src, "material_tex_guid", tex_guid))
            {
                auto filepath = ee0::AssetsMap::Instance()->QueryFilepath(tex_guid);
                if (!filepath.empty())
                {
                    sm::vec2 pos;
                    auto& st = bp_node->GetStyle();
                    pos.x = src.x - 100;
                    pos.y = -(src.y + st.height * 0.5f) + 50;

                    auto child = std::make_shared<sg::node::Tex2DAsset>();
                    child->SetImagePath(filepath);
                    CreateSceneNode(child, pos);

                    bp::make_connecting(child->GetAllOutput()[0], bp_node->GetAllInput()[0]);
                }
            }
        }
            break;
        case aseimp::NodeClass::SampleTriplanar:
            bp_node = std::make_shared<sg::node::SampleTriplanar>();
            break;
        case aseimp::NodeClass::UnpackScaleNormal:
            bp_node = std::make_shared<sg::node::UnpackScaleNormal>();
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
        {
            auto get_var = std::make_shared<bp::node::GetReference>();

            std::string name;
            if (QueryString(src, "name", name)) {
                get_var->SetName(name);
            }

            bp_node = get_var;
        }
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
        case aseimp::NodeClass::VertexToFragment:
            bp_node = std::make_shared<sg::node::VertexToFragment>();
            break;
        case aseimp::NodeClass::EncodeFloatRGBA:
            bp_node = std::make_shared<sg::node::EncodeFloatRGBA>();
            break;
        case aseimp::NodeClass::DecodeFloatRGBA:
            bp_node = std::make_shared<sg::node::DecodeFloatRGBA>();
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
            if (QueryString(src, "text", text))
            {
                if (text.empty()) {
                    if (QueryString(src, "title", text)) {
                        text = cpputil::StringHelper::UTF8ToGBK(text.c_str());
                        comm->SetCommentText(text);
                    }
                }
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
            rttr::type enum_type = rttr::type::get<aseimp::NodeClass>();
            assert(enum_type && enum_type.is_enumeration());
            auto enum_node_cls = enum_type.get_enumeration();
            auto name = enum_node_cls.value_to_name(src.cls).to_string();
            printf("Create BP node fail: %s\n", name.c_str());
            continue;
        }

        sm::vec2 pos;
        auto& st = bp_node->GetStyle();
        pos.x = src.x + st.width * 0.5f;
        pos.y = -(src.y + st.height * 0.5f);

        CreateSceneNode(bp_node, pos);

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
            printf("find node fail: %d to %d\n", node_from, node_to);
            continue;
        }

        //auto from_type = from->second.dst->get_type();
        //if (from_type == rttr::type::get<sg::node::Vector2>() ||
        //    from_type == rttr::type::get<sg::node::Vector3>() ||
        //    from_type == rttr::type::get<sg::node::Vector4>())
        //{
        //    assert(port_to == 0);
        //}

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

void ASEImporter::CreateSceneNode(const bp::NodePtr& bp_node, const sm::vec2& pos)
{
    auto dst = std::make_shared<n0::SceneNode>();
    dst->AddUniqueComp<n0::CompIdentity>();

    auto& cnode = dst->AddUniqueComp<bp::CompNode>();
    cnode.SetNode(bp_node);

    auto& style = bp_node->GetStyle();
    dst->AddUniqueComp<n2::CompBoundingBox>(
        sm::rect(style.width, style.height)
    );

    auto& ctrans = dst->AddUniqueComp<n2::CompTransform>();
    ctrans.SetPosition(*dst, pos);

    m_scene_nodes.push_back(dst);
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