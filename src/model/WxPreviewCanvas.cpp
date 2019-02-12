#include "model/WxPreviewCanvas.h"
#include "model/WxPreviewPanel.h"

#include <ee0/SubjectMgr.h>

#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <model/Model.h>
#include <easygui/ImGui.h>
#include <tessellation/Palette.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/MaterialMgr.h>
#include <facade/EasyGUI.h>
#include <facade/Facade.h>

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

	InitGui();
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
	params.type = pt3::RenderParams::DRAW_MESH;

	auto& cmodel_inst = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst  = cmodel_inst.GetModel();
	if (!model_inst) {
		return;
	}

    pt0::RenderContext ctx;
    ctx.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    assert(wc);
    ctx.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    ctx.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

    auto& cmodel = m_obj->GetSharedComp<n3::CompModel>();
    auto& mats = cmodel.GetAllMaterials();
    pt3::RenderSystem::Instance()->DrawModel(*model_inst, mats, params, ctx);
}

void WxPreviewCanvas::DrawGUI() const
{
	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
	auto sz = wc->GetScreenSize();
	PrepareDrawGui(sz.x, sz.y);

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