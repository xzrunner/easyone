#include "anim/WxTimelinePanel.h"
#include "anim/WxLayersToolbar.h"
#include "anim/WxLayersScrolled.h"
#include "anim/WxTimeScaleCtrl.h"
#include "anim/WxTimeStageScrolled.h"
#include "anim/MessageHelper.h"

#include <ee0/SubjectMgr.h>

#include <wx/splitter.h>
#include <wx/sizer.h>

namespace eone
{
namespace anim
{

WxTimelinePanel::WxTimelinePanel(wxWindow* parent, n2::CompAnim& canim,
	                             n2::CompAnimInst& canim_inst,
	                             const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent)
{
	InitLayout(canim, canim_inst, sub_mgr);

	MessageHelper::SetCurrFrame(*sub_mgr, 0, 0);
}

void WxTimelinePanel::InitLayout(n2::CompAnim& canim, n2::CompAnimInst& canim_inst,
	                             const ee0::SubjectMgrPtr& sub_mgr)
{
	wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);

	wxWindow* left_panel = new wxPanel(splitter);
	wxWindow* right_panel = new wxPanel(splitter);
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new WxLayersToolbar(left_panel, canim, canim_inst, sub_mgr), 1, wxEXPAND);
		sizer->Add(new WxLayersScrolled(left_panel, canim, sub_mgr), 99, wxEXPAND);
		left_panel->SetSizer(sizer);
	}
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->Add(new WxTimeScaleCtrl(right_panel, canim, canim_inst, sub_mgr), 0, wxEXPAND);
		sizer->Add(new WxTimeStageScrolled(right_panel, canim, canim_inst, sub_mgr), 99, wxEXPAND);
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