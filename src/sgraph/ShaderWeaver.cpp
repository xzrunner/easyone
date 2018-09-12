#include "sgraph/ShaderWeaver.h"

#include <blueprint/Node.h>
#include <blueprint/Pins.h>
#include <blueprint/Connecting.h>
#include <blueprint/CompNode.h>
#include <shadergraph/Pins.h>
#include <shadergraph/node/Constant1.h>
#include <shadergraph/node/Constant2.h>
#include <shadergraph/node/Constant3.h>
#include <shadergraph/node/Constant4.h>
#include <shadergraph/node/Input.h>
#include <shadergraph/node/Add.h>
#include <shadergraph/node/Subtract.h>
#include <shadergraph/node/Multiply.h>
#include <shadergraph/node/Divide.h>
#include <shadergraph/node/Phong.h>
#include <shadergraph/node/TextureSample.h>
#include <shadergraph/node/TextureObject.h>

#include <sw/Evaluator.h>
#include <sw/node/Vector1.h>
#include <sw/node/Vector2.h>
#include <sw/node/Vector3.h>
#include <sw/node/Vector4.h>
#include <sw/node/Add.h>
#include <sw/node/Subtract.h>
#include <sw/node/Multiply.h>
#include <sw/node/Divide.h>
#include <sw/node/Phong.h>
#include <sw/node/Input.h>
#include <sw/node/Output.h>
#include <sw/node/Uniform.h>
#include <sw/node/PositionTrans2.h>
#include <sw/node/FragPosTrans.h>
#include <sw/node/NormalTrans.h>
#include <sw/node/Tex2DSample.h>

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <unirender/Shader.h>
#include <node0/SceneNode.h>
#include <facade/Image.h>

#include <assert.h>

namespace
{

const bp::Node& get_input_node(const bp::Node& src, int idx)
{
	auto& to_port = src.GetAllInput()[idx];
	auto& conns = to_port->GetConnecting();
	assert(conns.size() == 1);
	auto& from_port = conns[0]->GetFrom();
	assert(from_port);

	return from_port->GetParent();
}

void add_vert_varying(std::vector<sw::NodePtr>& nodes, std::vector<sw::NodePtr>& cache_nodes, const std::string& name, uint32_t type)
{
	auto vert_in = std::make_shared<sw::node::Input>(name, type);
	auto vert_out = std::make_shared<sw::node::Output>("v_" + name, type);
	sw::make_connecting({ vert_in, 0 }, { vert_out, 0 });
	nodes.push_back(vert_out);

	cache_nodes.push_back(vert_in);
}

void debug_print(const sw::Evaluator& vert, const sw::Evaluator& frag)
{
	printf("//////////////////////////////////////////////////////////////////////////\n");
	printf("%s\n", vert.GetShaderStr().c_str());
	printf("//////////////////////////////////////////////////////////////////////////\n");
	printf("%s\n", frag.GetShaderStr().c_str());
	printf("//////////////////////////////////////////////////////////////////////////\n");
}

}

namespace eone
{
namespace sgraph
{

ShaderWeaver::ShaderWeaver(const std::vector<n0::SceneNodePtr>& nodes,
	                       const std::string& type)
{
	bp::NodePtr final_node = nullptr;
	for (auto& node : nodes)
	{
		assert(node->HasUniqueComp<bp::CompNode>());
		auto& bp_node = node->GetUniqueComp<bp::CompNode>().GetNode();
		assert(bp_node);
		if (bp_node->TypeName() == type) {
			final_node = bp_node;
		}
	}
	assert(final_node);

	if (type == sg::node::Phong::TYPE_NAME)
	{
		// layout
		m_layout.push_back(ur::VertexAttrib("position", 3, 4, 32, 0));
		m_layout.push_back(ur::VertexAttrib("normal",   3, 4, 32, 12));
		m_layout.push_back(ur::VertexAttrib("texcoord", 2, 4, 32, 24));

		// vert

		auto projection = std::make_shared<sw::node::Uniform>("u_projection", sw::t_mat4);
		auto view       = std::make_shared<sw::node::Uniform>("u_view",       sw::t_mat4);
		auto model      = std::make_shared<sw::node::Uniform>("u_model",      sw::t_mat4);
		m_cached_nodes.push_back(projection);
		m_cached_nodes.push_back(view);
		m_cached_nodes.push_back(model);

		auto position = std::make_shared<sw::node::Input>("position", sw::t_flt4);
		auto normal   = std::make_shared<sw::node::Input>("normal",   sw::t_nor3);
		m_cached_nodes.push_back(position);
		m_cached_nodes.push_back(normal);

		auto pos_trans = std::make_shared<sw::node::PositionTrans2>();
		sw::make_connecting({ projection, 0 }, { pos_trans, sw::node::PositionTrans2::IN_PROJ });
		sw::make_connecting({ view, 0 },       { pos_trans, sw::node::PositionTrans2::IN_VIEW });
		sw::make_connecting({ model, 0 },      { pos_trans, sw::node::PositionTrans2::IN_MODEL });
		sw::make_connecting({ position, 0 },   { pos_trans, sw::node::PositionTrans2::IN_POS });
		m_vert_nodes.push_back(pos_trans);

		auto frag_pos_trans = std::make_shared<sw::node::FragPosTrans>();
		sw::make_connecting({ model, 0 },    { frag_pos_trans, sw::node::FragPosTrans::IN_MODEL });
		sw::make_connecting({ position, 0 }, { frag_pos_trans, sw::node::FragPosTrans::IN_POS });
		m_vert_nodes.push_back(frag_pos_trans);

		auto norm_trans = std::make_shared<sw::node::NormalTrans>();
		sw::make_connecting({ model, 0 }, { norm_trans, sw::node::NormalTrans::IN_MODEL });
		sw::make_connecting({ normal, 0 }, { norm_trans, sw::node::NormalTrans::IN_NORM });
		m_vert_nodes.push_back(norm_trans);

		add_vert_varying(m_vert_nodes, m_cached_nodes, "frag_pos", sw::t_flt3);
		add_vert_varying(m_vert_nodes, m_cached_nodes, "normal",   sw::t_nor3);
		add_vert_varying(m_vert_nodes, m_cached_nodes, "texcoord", sw::t_uv);

		// frag

		auto phong = CreateWeaverNode(*final_node);

		auto frag_in_pos = std::make_shared<sw::node::Input>("v_frag_pos", sw::t_flt3);
		auto frag_in_nor = std::make_shared<sw::node::Input>("v_normal", sw::t_nor3);
		m_cached_nodes.push_back(frag_in_pos);
		m_cached_nodes.push_back(frag_in_nor);
		sw::make_connecting({ frag_in_pos, 0 }, { phong, sw::node::Phong::IN_FRAG_POS });
		sw::make_connecting({ frag_in_nor, 0 }, { phong, sw::node::Phong::IN_NORMAL });

		m_frag_node = phong;
	}
}

std::shared_ptr<ur::Shader> ShaderWeaver::CreateShader() const
{
	sw::Evaluator vert(m_vert_nodes, sw::ST_VERT);
	sw::Evaluator frag({ m_frag_node }, sw::ST_FRAG);

	debug_print(vert, frag);

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	auto shader = std::make_shared<ur::Shader>(&rc, vert.GetShaderStr().c_str(),
		frag.GetShaderStr().c_str(), m_texture_names, m_layout);

	shader->SetUsedTextures(m_texture_ids);

	assert(shader);
	return shader;
}

sw::NodePtr ShaderWeaver::CreateWeaverNode(const bp::Node& node)
{
	sw::NodePtr dst = nullptr;

	int id = node.TypeID();
	if (id == bp::GetNodeTypeID<sg::node::Phong>())
	{
		dst = std::make_shared<sw::node::Phong>();

		auto& src = static_cast<const sg::node::Phong&>(node);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_LIT_POSITION), 0 },
			{ dst, sw::node::Phong::IN_LIGHT_POSITION }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_LIT_AMBIENT), 0 },
			{ dst, sw::node::Phong::IN_LIGHT_AMBIENT }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_LIT_DIFFUSE), 0 },
			{ dst, sw::node::Phong::IN_LIGHT_DIFFUSE }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_LIT_SPECULAR), 0 },
			{ dst, sw::node::Phong::IN_LIGHT_SPECULAR }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_MAT_DIFFUSE), 0 },
			{ dst, sw::node::Phong::IN_MATERIAL_DIFFUSE }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_MAT_SPECULAR), 0 },
			{ dst, sw::node::Phong::IN_MATERIAL_SPECULAR }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_MAT_SHININESS), 0 },
			{ dst, sw::node::Phong::IN_MATERIAL_SHININESS }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_MAT_EMISSION), 0 },
			{ dst, sw::node::Phong::IN_MATERIAL_EMISSION }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::Phong::ID_VIEW_POS), 0 },
			{ dst, sw::node::Phong::IN_VIEW_POS }
		);
	}
	else if (id == bp::GetNodeTypeID<sg::node::Constant1>())
	{
		auto& src = static_cast<const sg::node::Constant1&>(node);
		dst = std::make_shared<sw::node::Vector1>(src.GetName(), src.GetValue());
	}
	else if (id == bp::GetNodeTypeID<sg::node::Constant2>())
	{
		auto& src = static_cast<const sg::node::Constant2&>(node);
		dst = std::make_shared<sw::node::Vector2>(src.GetName(), src.GetValue());
	}
	else if (id == bp::GetNodeTypeID<sg::node::Constant3>())
	{
		auto& src = static_cast<const sg::node::Constant3&>(node);
		dst = std::make_shared<sw::node::Vector3>(src.GetName(), src.GetValue());
	}
	else if (id == bp::GetNodeTypeID<sg::node::Constant4>())
	{
		auto& src = static_cast<const sg::node::Constant4&>(node);
		dst = std::make_shared<sw::node::Vector4>(src.GetName(), src.GetValue());
	}
	else if (id == bp::GetNodeTypeID<sg::node::Input>())
	{
		auto& src = static_cast<const sg::node::Input&>(node);
		uint32_t type = 0;
		switch (src.GetType())
		{
		case sg::PINS_TEXTURE2D:
			type = sw::t_tex2d;
			break;
		case sg::PINS_VECTOR2:
			type = sw::t_flt2;
			break;
		default:
			// todo
			assert(0);
		}
		dst = std::make_shared<sw::node::Input>(src.GetName(), type);
	}
	else if (id == bp::GetNodeTypeID<sg::node::Add>())
	{
		auto& src = static_cast<const sg::node::Add&>(node);
		dst = std::make_shared<sw::node::Add>();
		sw::make_connecting({ CreateInputChild(src, 0), 0 }, { dst, sw::node::Add::IN_A });
		sw::make_connecting({ CreateInputChild(src, 1), 0 }, { dst, sw::node::Add::IN_B });
	}
	else if (id == bp::GetNodeTypeID<sg::node::Subtract>())
	{
		auto& src = static_cast<const sg::node::Subtract&>(node);
		dst = std::make_shared<sw::node::Subtract>();
		sw::make_connecting({ CreateInputChild(src, 0), 0 }, { dst, sw::node::Subtract::IN_A });
		sw::make_connecting({ CreateInputChild(src, 1), 0 }, { dst, sw::node::Subtract::IN_B });
	}
	else if (id == bp::GetNodeTypeID<sg::node::Multiply>())
	{
		auto& src = static_cast<const sg::node::Multiply&>(node);
		dst = std::make_shared<sw::node::Multiply>();
		sw::make_connecting({ CreateInputChild(src, 0), 0 }, { dst, sw::node::Multiply::IN_A });
		sw::make_connecting({ CreateInputChild(src, 1), 0 }, { dst, sw::node::Multiply::IN_B });
	}
	else if (id == bp::GetNodeTypeID<sg::node::Divide>())
	{
		auto& src = static_cast<const sg::node::Divide&>(node);
		dst = std::make_shared<sw::node::Divide>();
		sw::make_connecting({ CreateInputChild(src, 0), 0 }, { dst, sw::node::Divide::IN_A });
		sw::make_connecting({ CreateInputChild(src, 1), 0 }, { dst, sw::node::Divide::IN_B });
	}
	else if (id == bp::GetNodeTypeID<sg::node::TextureSample>())
	{
		auto& src = static_cast<const sg::node::TextureSample&>(node);
		dst = std::make_shared<sw::node::Tex2DSample>();
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::TextureSample::ID_TEX), 0 },
			{ dst, sw::node::Tex2DSample::IN_TEX }
		);
		sw::make_connecting(
			{ CreateInputChild(src, sg::node::TextureSample::ID_UV), 0 },
			{ dst, sw::node::Tex2DSample::IN_UV }
		);
	}
	else if (id == bp::GetNodeTypeID<sg::node::TextureObject>())
	{
		auto& src = static_cast<const sg::node::TextureObject&>(node);
		m_texture_names.push_back(src.GetName());
		auto& img = src.GetImage();
		if (img) {
			m_texture_ids.push_back(img->GetTexID());
		}
		dst = std::make_shared<sw::node::Uniform>(src.GetName(), sw::t_tex2d);
	}
	else
	{
		assert(0);
	}

	if (dst) {
		m_cached_nodes.push_back(dst);
	}

	return dst;
}

sw::NodePtr ShaderWeaver::CreateInputChild(const bp::Node& node, int input_idx)
{
	auto& to_port = node.GetAllInput()[input_idx];
	auto& conns = to_port->GetConnecting();
	assert(conns.size() == 1);
	auto& from_port = conns[0]->GetFrom();
	assert(from_port);
	return CreateWeaverNode(from_port->GetParent());
}

}
}