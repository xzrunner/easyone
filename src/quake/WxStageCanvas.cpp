#include "quake/WxStageCanvas.h"
#include "quake/DrawFaceShader.h"
#include "quake/DrawEdgeShader.h"
#include "quake/face.glsl"
#include "quake/edge.glsl"

#include <unirender/VertexAttrib.h>
#include <unirender/Blackboard.h>
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting3/EffectsManager.h>
#include <painting3/PerspCam.h>
#include <node3/RenderSystem.h>
#include <facade/RenderContext.h>

namespace
{

std::shared_ptr<ur::Shader> FACE_SHADER = nullptr;
std::shared_ptr<ur::Shader> EDGE_SHADER = nullptr;

}

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
	if (!FACE_SHADER || !EDGE_SHADER) {
		InitShaders();
	}

	// draw cross

	tess::Painter pt;

	auto cam_mat = m_camera->GetViewMat() * m_camera->GetProjectionMat();
	auto trans3d = [&](const sm::vec3& pos3)->sm::vec2 {
		return GetViewport().TransPosProj3ToProj2(pos3, cam_mat);
	};

	const float len = 1;
	pt.AddLine3D({ -len, 0, 0 }, { len, 0, 0 }, trans3d, 0xff0000ff, 2);
	pt.AddLine3D({ 0, -len, 0 }, { 0, len, 0 }, trans3d, 0xff00ff00, 2);
	pt.AddLine3D({ 0, 0, -len }, { 0, 0, len }, trans3d, 0xffff0000, 2);

	const float radius = 0.1f;
	pt.AddCircleFilled(trans3d(sm::vec3(len, 0, 0)), radius, 0xff0000ff);
	pt.AddCircleFilled(trans3d(sm::vec3(0, len, 0)), radius, 0xff00ff00);
	pt.AddCircleFilled(trans3d(sm::vec3(0, 0, len)), radius, 0xffff0000);

	pt2::RenderSystem::DrawPainter(pt);
}

void WxStageCanvas::DrawForeground() const
{
	auto& rc = const_cast<ee0::RenderContext&>(GetRenderContext()).facade_rc;
	auto& ur_rc = rc->GetUrRc();

	// pass 1 draw face
	auto& cam = GetCamera();
	if (cam->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
	{
		pt3::EffectsManager::Instance()->SetUserEffect(
			std::static_pointer_cast<ur::Shader>(FACE_SHADER));
		pt3::EffectsManager::Instance()->Use(model::EFFECT_USER);
		DrawNodes(pt3::RenderParams::DRAW_MESH);
	}

	// pass 2 draw edge
	pt3::EffectsManager::Instance()->SetUserEffect(
		std::static_pointer_cast<ur::Shader>(EDGE_SHADER));
	pt3::EffectsManager::Instance()->Use(model::EFFECT_USER);
	DrawNodes(pt3::RenderParams::DRAW_BORDER_MESH);
}

void WxStageCanvas::InitShaders() const
{
	CU_VEC<ur::VertexAttrib> layout;
	layout.push_back(ur::VertexAttrib("position", 3, 4, 32, 0));
	layout.push_back(ur::VertexAttrib("texcoord", 2, 4, 32, 24));

	std::vector<std::string> textures;

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	FACE_SHADER = std::make_shared<DrawFaceShader>(
		&rc, face_vs, face_fs, textures, layout);
	EDGE_SHADER = std::make_shared<DrawEdgeShader>(
		&rc, edge_vs, edge_fs, textures, layout);
}

}
}