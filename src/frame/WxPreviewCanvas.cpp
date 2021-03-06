#include "frame/WxPreviewCanvas.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxStagePage.h"

#include <ee0/SubjectMgr.h>
#include <ee0/EditOP.h>
#include <ee0/color_config.h>

#include <unirender/RenderState.h>
#include <painting2/OrthoCamera.h>
#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>
#include <painting2/RenderSystem.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/RenderSystem.h>
#include <node2/CompUniquePatch.h>
#else
#include <entity2/SysRender.h>
#endif // GAME_OBJ_ECS
#include <facade/RenderContext.h>
#include <facade/RenderContext.h>
#include <tessellation/Painter.h>

namespace eone
{

WxPreviewCanvas::WxPreviewCanvas(const ur::Device& dev, WxPreviewPanel* stage,
                                 ECS_WORLD_PARAM const ee0::RenderContext& rc)
	: ee0::WxStageCanvas(dev, stage, stage->GetImpl(), std::make_shared<pt2::OrthoCamera>(sm::vec2(0, 0), 2.0f), &rc, nullptr, HAS_2D)
	, m_stage(stage)
	ECS_WORLD_SELF_ASSIGN
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

void WxPreviewCanvas::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_SET_CANVAS_DIRTY, this);
}

void WxPreviewCanvas::OnDrawSprites() const
{
	ee0::RenderContext::Reset2D(true);

	const float hw = 1024 * 0.5f;
	const float hh = 768 * 0.5f;
	float line_width = 2.0f;
	if (m_camera->TypeID() == pt0::GetCamTypeID<pt2::OrthoCamera>()) {
		line_width *= std::dynamic_pointer_cast<pt2::OrthoCamera>(m_camera)->GetScale();
	}
	tess::Painter pt;
	pt.AddRect({ -hw, -hh }, { hw, hh }, 0xffffffff, line_width);

    auto& ctx = *GetRenderContext().ur_ctx;
    ur::RenderState rs;
	pt2::RenderSystem::DrawPainter(m_dev, ctx, rs, pt);

	ee0::VariantSet vars;
	ee0::Variant var;
	var.m_type = ee0::VT_LONG;
	var.m_val.l = WxStagePage::TRAV_DRAW_PREVIEW;
	vars.SetVariant("type", var);

#ifndef GAME_OBJ_ECS
	n2::RenderParams rp;
	rp.SetEditMode(false);
#else
	e2::RenderParams rp;
#endif // GAME_OBJ_ECS
	m_stage->GetStagePage().Traverse([&](const ee0::GameObj& obj)->bool
	{
#ifndef GAME_OBJ_ECS
		if (obj->HasUniqueComp<n2::CompUniquePatch>())
		{
			auto patch = &obj->GetUniqueComp<n2::CompUniquePatch>();
			patch->Rewind();
			rp.SetPatch(patch);
		}

		n2::RenderSystem::Instance()->Draw(m_dev, ctx, rs, obj, rp);
#else
		e2::SysRender::Draw(m_world, obj, rp);
#endif // GAME_OBJ_ECS

		return true;
	}, vars);

	auto& op = m_stage->GetImpl().GetEditOP();
	if (op) {
		op->OnDraw(m_dev, ctx);
	}
}

}