#include "quake/WxStageCanvas.h"

#include <unirender/VertexAttrib.h>
#include <unirender/Blackboard.h>
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting3/PerspCam.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <drawing3/DrawFaceShader.h>
#include <drawing3/DrawEdgeShader.h>
#include <drawing3/face.glsl>
#include <drawing3/edge.glsl>
#include <drawing3/EditOpMgr.h>
#include <node3/RenderSystem.h>
#include <facade/RenderContext.h>
#include <facade/Facade.h>
#include <easygui/Context.h>
#include <easygui/ImGui.h>

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
	                         const ee0::RenderContext& rc, dw3::EditOpMgr& op_mgr)
	: WxStageCanvas3D(stage, ECS_WORLD_VAR rc, true)
	, m_op_mgr(op_mgr)
{
	InitGui();
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
	DrawSceneNodes();
	DrawGUI();
}

void WxStageCanvas::InitShaders() const
{
	CU_VEC<ur::VertexAttrib> layout;
	layout.push_back(ur::VertexAttrib("position", 3, 4, 32, 0));
	layout.push_back(ur::VertexAttrib("texcoord", 2, 4, 32, 24));

	std::vector<std::string> textures;

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	FACE_SHADER = std::make_shared<dw3::DrawFaceShader>(
		&rc, face_vs, face_fs, textures, layout);
	EDGE_SHADER = std::make_shared<dw3::DrawEdgeShader>(
		&rc, edge_vs, edge_fs, textures, layout);
}

void WxStageCanvas::DrawSceneNodes() const
{
	auto& rc = const_cast<ee0::RenderContext&>(GetRenderContext()).facade_rc;
	auto& ur_rc = rc->GetUrRc();

	// pass 1 draw face
	auto& cam = GetCamera();
	if (cam->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
	{
        FACE_SHADER->Use();
		DrawNodes(pt3::RenderParams::DRAW_MESH);
	}

	// pass 2 draw edge
    EDGE_SHADER->Use();
	DrawNodes(pt3::RenderParams::DRAW_BORDER_MESH);
}

void WxStageCanvas::DrawGUI() const
{
	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
	auto sz = wc->GetScreenSize();
	PrepareDrawGui(sz.x, sz.y);

	auto& ctx = GetWidnowContext().egui;

	int uid = 1;

	ctx->BeginDraw();

	bool dirty = facade::Facade::Instance()->IsLastFrameDirty();
	//if (m_last_screen_sz != sz) {
	//	dirty = true;
	//	m_last_screen_sz = sz;
	//}

	float x = sz.x * 0.5f - 150;
	float y = sz.y * 0.5f - 40;
	float h = 25;
	auto op_type = m_op_mgr.GetCurrOp();
	if (egui::radio_button(uid++, "DRAW", op_type == dw3::EditOpMgr::Operator::DRAW, x, y, *ctx, dirty)) {
		m_op_mgr.SetCurrOp(dw3::EditOpMgr::Operator::DRAW);
	}
	y -= h;
	if (egui::radio_button(uid++, "ARRANGE", op_type == dw3::EditOpMgr::Operator::ARRANGE, x, y, *ctx, dirty)) {
		m_op_mgr.SetCurrOp(dw3::EditOpMgr::Operator::ARRANGE);
	}
	y -= h;
	if (egui::radio_button(uid++, "VERTEX", op_type == dw3::EditOpMgr::Operator::VERTEX, x, y, *ctx, dirty)) {
		m_op_mgr.SetCurrOp(dw3::EditOpMgr::Operator::VERTEX);
	}
	y -= h;
	if (egui::radio_button(uid++, "EDGE", op_type == dw3::EditOpMgr::Operator::EDGE, x, y, *ctx, dirty)) {
		m_op_mgr.SetCurrOp(dw3::EditOpMgr::Operator::EDGE);
	}
	y -= h;
	if (egui::radio_button(uid++, "FACE", op_type == dw3::EditOpMgr::Operator::FACE, x, y, *ctx, dirty)) {
		m_op_mgr.SetCurrOp(dw3::EditOpMgr::Operator::FACE);
	}
	y -= h;

	ctx->EndDraw();

	// todo
	const float dt = 0.033f;
	ctx->Update(dt);
}

}
}