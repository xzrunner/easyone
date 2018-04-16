#include "frame/WxPreviewCanvas.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxStagePage.h"

#include <ee0/SubjectMgr.h>
#include <ee0/EditOP.h>
#include <ee0/color_config.h>

#include <painting2/OrthoCamera.h>
#include <painting2/PrimitiveDraw.h>
#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/RenderSystem.h>
#include <node2/CompUniquePatch.h>
#else
#include <entity2/SysRender.h>
#endif // GAME_OBJ_ECS

namespace eone
{

WxPreviewCanvas::WxPreviewCanvas(WxPreviewPanel* stage, ECS_WORLD_PARAM
	                             const ee0::RenderContext& rc)
	: ee0::WxStageCanvas(stage, stage->GetImpl(), &rc, nullptr, HAS_2D)
	, m_stage(stage)
	ECS_WORLD_SELF_ASSIGN
{
	auto cam = std::make_shared<pt2::OrthoCamera>();
	cam->Set(sm::vec2(0, 0), 2);
	m_cam = cam;

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

void WxPreviewCanvas::OnSize(int w, int h)
{
	auto& wc = pt2::Blackboard::Instance()->GetWindowContext();
	if (wc)
	{
		wc->SetViewport(0, 0, w, h);
		wc->SetScreen(w, h);
		wc->SetProjection(w, h);
	}
}

void WxPreviewCanvas::OnDrawSprites() const
{
	pt2::PrimitiveDraw::SetColor(ee0::WHITE);
	pt2::PrimitiveDraw::Rect(nullptr, sm::vec2(0, 0), 1024, 768, false);

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

		n2::RenderSystem::Draw(obj, rp);
#else
		e2::SysRender::Draw(m_world, obj, rp);
#endif // GAME_OBJ_ECS

		return true;
	}, vars);

	auto& op = m_stage->GetImpl().GetEditOP();
	if (op) {
		op->OnDraw();
	}
}

}