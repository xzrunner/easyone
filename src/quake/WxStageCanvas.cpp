#include "quake/WxStageCanvas.h"
#include "quake/DrawFaceShader.h"
#include "quake/DrawEdgeShader.h"
#include "quake/face.glsl"
#include "quake/edge.glsl"

#include <unirender/VertexAttrib.h>
#include <unirender/Blackboard.h>
#include <shaderlab/Blackboard.h>
#include <shaderlab/ShaderType.h>
#include <shaderlab/RenderContext.h>
#include <painting2/PrimitiveDraw.h>
#include <painting3/EffectsManager.h>
#include <painting3/PrimitiveDraw.h>
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

	static const int LEN = 100;
	pt2::PrimitiveDraw::PointSize(5);
	pt2::PrimitiveDraw::LineWidth(2);
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
	auto& cam = GetCamera();
	if (cam->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
	{
		pt3::EffectsManager::Instance()->SetUserEffect(
			std::static_pointer_cast<ur::Shader>(FACE_SHADER));
		pt3::EffectsManager::Instance()->Use(pt3::EffectsManager::EFFECT_USER);
		DrawNodes(pt3::RenderParams::DRAW_MESH);
	}

	// pass 2 draw edge
	pt2::PrimitiveDraw::LineWidth(1);
	pt3::EffectsManager::Instance()->SetUserEffect(
		std::static_pointer_cast<ur::Shader>(EDGE_SHADER));
	pt3::EffectsManager::Instance()->Use(pt3::EffectsManager::EFFECT_USER);
	DrawNodes(pt3::RenderParams::DRAW_BORDER_MESH);
}

void WxStageCanvas::InitShaders() const
{
	// flush shader status
	auto& shader_mgr = sl::Blackboard::Instance()->GetRenderContext().GetShaderMgr();
	shader_mgr.SetShader(sl::EXTERN_SHADER);
	//shader_mgr.BindRenderShader(nullptr, sl::EXTERN_SHADER);

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