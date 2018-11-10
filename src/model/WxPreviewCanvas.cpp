#include "model/WxPreviewCanvas.h"
#include "model/WxPreviewPanel.h"
#include "model/RenderSystem.h"

#include <ee0/SubjectMgr.h>

#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <model/Model.h>
#include <easygui/ImGui.h>
#include <facade/DTex.h>
#include <facade/EasyGUI.h>
#include <facade/Facade.h>
#include <tessellation/Palette.h>
#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <painting0/Shader.h>
#include <painting2/Shader.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <rendergraph/RenderMgr.h>
#include <rendergraph/SpriteRenderer.h>

namespace eone
{
namespace model
{

WxPreviewCanvas::WxPreviewCanvas(WxPreviewPanel* stage, ECS_WORLD_PARAM
	                             const ee0::RenderContext& rc, const ee0::GameObj& obj)
	: ee3::WxStageCanvas(stage, &rc, nullptr, true)
	, m_obj(obj)
{
	RegisterMsg(*stage->GetSubjectMgr());

	// init egui
	facade::EasyGUI::Instance();
	auto& wc = const_cast<ee0::WindowContext&>(GetWidnowContext());
	wc.egui = std::make_shared<egui::Context>();
	egui::style_colors_dark(wc.egui->style);
}

void WxPreviewCanvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_SET_CANVAS_DIRTY:
		SetDirty();
		break;
	}
}

void WxPreviewCanvas::DrawBackground() const
{
}

void WxPreviewCanvas::DrawForeground() const
{
	if (m_obj->HasSharedComp<n3::CompModel>()) {
		DrawModel();
	}
	DrawGUI();
}

bool WxPreviewCanvas::OnUpdate()
{
	auto& cmodel = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmodel.GetModel();
	if (!model_inst) {
		return false;
	}

	static int frame = 0;
	model_inst->SetFrame(frame * model_inst->GetModel()->anim_speed * m_anim_speed);
	++frame;

	return true;
}

void WxPreviewCanvas::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_SET_CANVAS_DIRTY, this);
}

void WxPreviewCanvas::DrawModel() const
{
	pt3::RenderParams params;
	params.mt   = m_camera->GetViewMat();
	params.type = pt3::RenderParams::DRAW_MESH;

	auto& cmodel     = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmodel.GetModel();
	if (!model_inst) {
		return;
	}

	RenderSystem::Instance()->DrawModel(*model_inst, params);
}

void WxPreviewCanvas::DrawGUI() const
{
	ee0::RenderContext::Reset2D();

	auto sr = std::static_pointer_cast<rg::SpriteRenderer>(
		rg::RenderMgr::Instance()->SetRenderer(rg::RenderType::SPRITE)
	);
	auto& palette = sr->GetPalette();

	auto shader = sr->GetShader();
	shader->Use();

	auto pt2_shader = std::dynamic_pointer_cast<pt2::Shader>(shader);
	assert(pt2_shader);
	pt2_shader->UpdateViewMat(sm::vec2(0, 0), 1.0f);

	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
	auto sz = wc->GetScreenSize();
	pt2_shader->UpdateProjMat(sz.x, sz.y);

	shader->UpdateModelMat(sm::mat4());

	ur::Blackboard::Instance()->GetRenderContext().BindTexture(facade::DTex::Instance()->GetSymCacheTexID(), 0);

	///

	auto& ctx = GetWidnowContext().egui;

	int uid = 1;

	ctx->BeginDraw();

	bool dirty = facade::Facade::Instance()->IsLastFrameDirty();
	if (m_last_screen_sz != sz) {
		dirty = true;
		m_last_screen_sz = sz;
	}
	egui::slider(uid++, "speed", &m_anim_speed, sz.x * 0.5f - 60, sz.y * 0.5f - 140, 128, 2, true, *ctx, dirty);

	ctx->EndDraw();

	// todo
	const float dt = 0.033f;
	ctx->Update(dt);
}

}
}