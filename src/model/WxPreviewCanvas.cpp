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
#include <rendergraph/RenderMgr.h>
#include <rendergraph/SpriteRenderer.h>

namespace eone
{
namespace model
{

WxPreviewCanvas::WxPreviewCanvas(WxPreviewPanel* stage, ECS_WORLD_PARAM
	                             const ee0::RenderContext& rc, const ee0::GameObj& obj)
	: ee3::WxStageCanvas(stage, &rc, nullptr, false)
	, m_obj(obj)
{
	RegisterMsg(*stage->GetSubjectMgr());
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
	model_inst->SetFrame(frame * model_inst->GetModel()->anim_speed);
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
	shader->SetMat4("u_model", sm::mat4().x);

	ur::Blackboard::Instance()->GetRenderContext().BindTexture(facade::DTex::Instance()->GetSymCacheTexID(), 0);

	///

	auto& ctx = facade::EasyGUI::Instance()->ctx;

	int uid = 1;

	ctx.BeginDraw();

	const bool dirty = facade::Facade::Instance()->IsLastFrameDirty();
	egui::slider(uid++, "speed", &m_anim_speed, 50, -100, 255, 2, true, ctx, dirty);

	ctx.EndDraw();

	// todo
	const float dt = 0.033f;
	ctx.Update(dt);
}

}
}