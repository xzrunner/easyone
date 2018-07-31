#include "quake/WxStageCanvas.h"
#include "quake/DrawFaceShader.h"
#include "quake/DrawEdgeShader.h"
#include "quake/face.glsl"
#include "quake/edge.glsl"

#include <unirender/VertexAttrib.h>
#include <unirender/Blackboard.h>
#include <painting2/PrimitiveDraw.h>
#include <painting3/EffectsManager.h>
#include <painting3/PrimitiveDraw.h>
#include <node3/RenderSystem.h>
#include <facade/RenderContext.h>

namespace eone
{
namespace quake
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: WxStageCanvas3D(stage, ECS_WORLD_VAR rc, true)
{
}

void WxStageCanvas::DrawBackground() const
{
	// todo: fail to put InitShaders() in ctor
	static bool inited = false;
	if (!inited) {
		InitShaders();
		inited = true;
	}

	static const int LEN = 100;
	pt2::PrimitiveDraw::PointSize(5);
	pt3::PrimitiveDraw::SetColor(0xff0000ff);
	pt3::PrimitiveDraw::Line(sm::vec3(-LEN, 0, 0), sm::vec3(LEN, 0, 0));
	pt3::PrimitiveDraw::Point(sm::vec3(LEN, 0, 0));
	pt3::PrimitiveDraw::SetColor(0xff00ff00);
	pt3::PrimitiveDraw::Line(sm::vec3(0, -LEN, 0), sm::vec3(0, LEN, 0));
	pt3::PrimitiveDraw::Point(sm::vec3(0, LEN, 0));
	pt3::PrimitiveDraw::SetColor(0xffff0000);
	pt3::PrimitiveDraw::Line(sm::vec3(0, 0, -LEN), sm::vec3(0, 0, LEN));
	pt3::PrimitiveDraw::Point(sm::vec3(0, 0, LEN));
}

void WxStageCanvas::DrawForeground() const
{
	auto& rc = const_cast<ee0::RenderContext&>(GetRenderContext()).facade_rc;
	auto& ur_rc = rc->GetUrRc();

	// pass 1 draw face
	pt3::EffectsManager::Instance()->SetUserEffect(
		std::static_pointer_cast<ur::Shader>(m_face_shader));
	pt3::EffectsManager::Instance()->Use(pt3::EffectsManager::EFFECT_USER);
	DrawNodes(n3::RenderParams::DRAW_MESH);

	// pass 2 draw edge
	pt3::EffectsManager::Instance()->SetUserEffect(
		std::static_pointer_cast<ur::Shader>(m_edge_shader));
	pt3::EffectsManager::Instance()->Use(pt3::EffectsManager::EFFECT_USER);
	DrawNodes(n3::RenderParams::DRAW_BORDER_MESH);
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