#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"
#include "frame/WxStagePage.h"

#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>

namespace eone
{

WxPreviewPanel::WxPreviewPanel(wxWindow* parent, ee0::SubjectMgr& sub_mgr, WxStagePage* stage)
	: ee0::WxEditPanel(parent)
	, m_sub_mgr(&sub_mgr)
	, m_stage(stage)
{
	RegisterMsg(*m_sub_mgr);
}

void WxPreviewPanel::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_STAGE_PAGE_CHANGING:
		StagePageChanging(variants);
		break;
	}
}

void WxPreviewPanel::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_STAGE_PAGE_CHANGING, this);
}

void WxPreviewPanel::StagePageChanging(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("new_page");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: new_page");
	GD_ASSERT(var.m_val.pv, "err new_page");
	auto new_page = static_cast<WxStagePage*>(var.m_val.pv);
	m_sub_mgr = &new_page->GetSubjectMgr();
	m_stage = new_page;

	RegisterMsg(*m_sub_mgr);

	auto canvas = std::dynamic_pointer_cast<WxPreviewCanvas>(
		GetImpl().GetCanvas());
	canvas->RegisterMsg(*m_sub_mgr);
	canvas->SetDirty();
}

}