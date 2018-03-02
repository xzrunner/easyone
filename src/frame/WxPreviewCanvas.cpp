#include "frame/WxPreviewCanvas.h"
#include "frame/WxPreviewPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/EditOP.h>
#include <ee0/color_config.h>
#include <ee0/WxStagePage.h>

#include <painting2/OrthoCamera.h>
#include <painting2/PrimitiveDraw.h>
#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>
#include <node2/RenderSystem.h>

namespace eone
{

WxPreviewCanvas::WxPreviewCanvas(WxPreviewPanel* stage, const ee0::RenderContext& rc)
	: ee0::WxStageCanvas(stage, stage->GetImpl(), &rc, nullptr, HAS_2D)
	, m_stage(stage)
{
	auto cam = std::make_shared<pt2::OrthoCamera>();
	cam->Set(sm::vec2(0, 0), 2);
	m_cam = cam;

	RegisterMsg(stage->GetSubjectMgr());
}

void WxPreviewCanvas::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
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
	var.m_type = ee0::VT_BOOL;
	var.m_val.bl = true;
	vars.SetVariant("preview", var);

	m_stage->GetStagePage().Traverse([&](const n0::SceneNodePtr& node)->bool {
		n2::RenderSystem::Draw(node, sm::Matrix2D());
		return true;
	}, vars);

	auto& op = m_stage->GetImpl().GetEditOP();
	if (op) {
		op->OnDraw();
	}
}

}