#include "anim/WxTimelinePanel.h"
#include "anim/WxLayersToolbar.h"
#include "anim/WxLayersPanel.h"
#include "anim/WxTimeScaleCtrl.h"
#include "anim/WxTimeStagePanel.h"

#include <ee0/SubjectMgr.h>

#include <wx/splitter.h>
#include <wx/sizer.h>

namespace eone
{
namespace anim
{

WxTimelinePanel::WxTimelinePanel(wxWindow* parent, const n2::CompAnim& canim)
	: wxPanel(parent)
{
	m_tl_sub_mgr = std::make_shared<ee0::SubjectMgr>();

	InitLayout(canim);
}
void WxTimelinePanel::InitLayout(const n2::CompAnim& canim)
{
	wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);

	wxWindow* left_panel = new wxPanel(splitter);
	wxWindow* right_panel = new wxPanel(splitter);
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new WxLayersToolbar(left_panel), 0, wxEXPAND);
		sizer->Add(new WxLayersPanel(left_panel, canim), 0, wxEXPAND);
		left_panel->SetSizer(sizer);
	}
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new WxTimeScaleCtrl(right_panel, canim, m_tl_sub_mgr), 0, wxEXPAND);
		sizer->Add(new WxTimeStagePanel(right_panel, canim, m_tl_sub_mgr), 1, wxEXPAND);
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