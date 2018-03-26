#include "anim/WxTimelinePanel.h"
#include "anim/WxLayersToolbar.h"
#include "anim/WxLayersPanel.h"
#include "anim/WxTimeScaleCtrl.h"
#include "anim/WxTimeStagePanel.h"
#include "anim/MessageHelper.h"
#include "anim/MessageID.h"

#include <ee0/SubjectMgr.h>

#include <guard/check.h>

#include <wx/splitter.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>

namespace
{

class WxScrolledLayers : public wxScrolledWindow, public ee0::Observer
{
public:
	WxScrolledLayers(wxWindow* parent, const n2::CompAnim& canim,
		const ee0::SubjectMgrPtr& sub_mgr)
		: wxScrolledWindow(parent)
	{
		ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);

		SetScrollbars(0, 1, 0, 10, 0, 0);
		InitLayout(canim, sub_mgr);
		
		sub_mgr->RegisterObserver(eone::anim::MSG_WND_SCROLL, this);
	}

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override
	{
		switch (msg)
		{
		case eone::anim::MSG_WND_SCROLL:
			{
				auto var = variants.GetVariant("y");
				GD_ASSERT(var.m_type == ee0::VT_LONG, "err val");
				Scroll(0, var.m_val.l);
			}
			break;
		}
	}

private:
	void InitLayout(const n2::CompAnim& canim, const ee0::SubjectMgrPtr& sub_mgr)
	{
		auto sizer = new wxBoxSizer(wxVERTICAL);
		auto layers = new eone::anim::WxLayersPanel(this, canim, sub_mgr);
		sizer->Add(layers, 1, wxEXPAND);
		SetSizer(sizer);
	}

}; // WxScrolledLayers

class WxScrolledTimeStage : public wxScrolledWindow
{
public:
	WxScrolledTimeStage(wxWindow* parent, const n2::CompAnim& canim,
		const ee0::SubjectMgrPtr& sub_mgr)
		: wxScrolledWindow(parent)
		, m_sub_mgr(sub_mgr)
	{
		SetScrollbars(1, 1, 20, 10, 0, 0);
		InitLayout(canim, sub_mgr);
	}

private:
	void InitLayout(const n2::CompAnim& canim, const ee0::SubjectMgrPtr& sub_mgr)
	{
		auto sizer = new wxBoxSizer(wxVERTICAL);
		auto layers = new eone::anim::WxTimeStagePanel(this, canim, sub_mgr);
		sizer->Add(layers, 1, wxEXPAND);
		SetSizer(sizer);
	}

	void OnScroll(wxScrollWinEvent& event)
	{
		auto pos = GetViewStart();

		ee0::VariantSet vars;

		ee0::Variant var_x;
		var_x.m_type = ee0::VT_LONG;
		var_x.m_val.l = pos.x;
		vars.SetVariant("x", var_x);

		ee0::Variant var_y;
		var_y.m_type = ee0::VT_LONG;
		var_y.m_val.l = pos.y;
		vars.SetVariant("y", var_y);

		m_sub_mgr->NotifyObservers(eone::anim::MSG_WND_SCROLL, vars);

		event.Skip();
	}

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	DECLARE_EVENT_TABLE()

}; // WxScrolledTimeStage

BEGIN_EVENT_TABLE(WxScrolledTimeStage, wxScrolledWindow)
	EVT_SCROLLWIN(WxScrolledTimeStage::OnScroll)
END_EVENT_TABLE()

}

namespace eone
{
namespace anim
{

WxTimelinePanel::WxTimelinePanel(wxWindow* parent, n2::CompAnim& canim)
	: wxPanel(parent)
{
	m_tl_sub_mgr = std::make_shared<ee0::SubjectMgr>();

	InitLayout(canim);

	MessageHelper::SetCurrFrame(*m_tl_sub_mgr, 0, 0);
}

void WxTimelinePanel::InitLayout(n2::CompAnim& canim)
{
	wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);

	wxWindow* left_panel = new wxPanel(splitter);
	wxWindow* right_panel = new wxPanel(splitter);
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new WxLayersToolbar(left_panel, canim, m_tl_sub_mgr), 1, wxEXPAND);
		sizer->Add(new WxScrolledLayers(left_panel, canim, m_tl_sub_mgr), 99, wxEXPAND);
		left_panel->SetSizer(sizer);
	}
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new WxTimeScaleCtrl(right_panel, canim, m_tl_sub_mgr), 0, wxEXPAND);
		sizer->Add(new WxScrolledTimeStage(right_panel, canim, m_tl_sub_mgr), 99, wxEXPAND);
		right_panel->SetSizer(sizer);
	}
	splitter->SetSashGravity(0.1f);
	splitter->SplitVertically(left_panel, right_panel);

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(splitter, 1, wxEXPAND);
	SetSizer(sizer);
}

}
}