#include "quake/WxStageCanvas.h"
#include "quake/DrawFaceShader.h"
#include "quake/DrawEdgeShader.h"
#include "quake/face.glsl"
#include "quake/edge.glsl"

#include <unirender/VertexAttrib.h>
#include <unirender/Shader.h>
#include <unirender/Blackboard.h>
#include <painting3/EffectsManager.h>

namespace eone
{
namespace quake
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: WxStageCanvas3D(stage, ECS_WORLD_VAR rc)
{
}

void WxStageCanvas::DrawForeground() const
{
	// todo: fail to put InitShaders() in ctor
	static bool inited = false;
	if (!inited) {
		InitShaders();
		inited = true;
	}

	// pass 1 draw face
	pt3::EffectsManager::Instance()->SetUserEffect(
		std::static_pointer_cast<ur::Shader>(m_face_shader));
	pt3::EffectsManager::Instance()->Use(pt3::EffectsManager::EFFECT_USER);
	DrawNodes();

	// pass 2 draw edge
	pt3::EffectsManager::Instance()->SetUserEffect(
		std::static_pointer_cast<ur::Shader>(m_edge_shader));
	pt3::EffectsManager::Instance()->Use(pt3::EffectsManager::EFFECT_USER);
	DrawNodes();
}

void WxStageCanvas::InitShaders() const
{
	CU_VEC<ur::VertexAttrib> layout;
	layout.push_back(ur::VertexAttrib("position", 3, 4, 32, 0));
	layout.push_back(ur::VertexAttrib("texcoord", 2, 4, 32, 24));

	std::vector<std::string> textures;

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	m_face_shader = std::make_shared<DrawFaceShader>(
		&rc, face_vs, face_fs, textures, layout);
	m_edge_shader = std::make_shared<DrawEdgeShader>(
		&rc, edge_vs, edge_fs, textures, layout);
}

}
}