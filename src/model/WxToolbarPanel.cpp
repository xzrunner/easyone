#include "model/WxToolbarPanel.h"
#include "model/WxStagePage.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxSkeletalTreeCtrl.h>

#include <node0/SceneNode.h>
#include <node3/CompModelInst.h>
#include <model/Model.h>

#include <wx/radiobox.h>

namespace eone
{
namespace model
{

WxToolbarPanel::WxToolbarPanel(wxWindow* parent, WxStagePage* stage)
	: wxPanel(parent)
	, m_stage(stage)
{
	InitLayout();
}

void WxToolbarPanel::InitLayout()
{
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	// top
	{
		wxArrayString choices;
		choices.Add("Rotate Joint");
		choices.Add("Translate Joint");
		choices.Add("IK");
		m_edit = new wxRadioBox(this, wxID_ANY, "edit_op", wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);
		Connect(m_edit->GetId(), wxEVT_COMMAND_RADIOBOX_SELECTED,
			wxCommandEventHandler(WxToolbarPanel::OnChangeEditType));
		sizer->Add(m_edit, 1, wxEXPAND);
	}
	// bottom
	{
		m_tree = new WxTreeScrolled(this, m_stage->GetSubjectMgr());
		sizer->Add(m_tree, 1, wxEXPAND);
	}
	SetSizer(sizer);
}

void WxToolbarPanel::OnChangeEditType(wxCommandEvent& event)
{
	int sel = event.GetSelection();
	switch (sel)
	{
	case 0:
		m_stage->SetEditOp(WxStagePage::OP_ROTATE_JOINT);
		break;
	case 1:
		m_stage->SetEditOp(WxStagePage::OP_TRANSLATE_JOINT);
		break;
	case 2:
		m_stage->SetEditOp(WxStagePage::OP_IK);
		break;
	}
	m_stage->GetSubjectMgr()->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

//////////////////////////////////////////////////////////////////////////
// class WxToolbarPanel::WxTreeScrolled
//////////////////////////////////////////////////////////////////////////

WxToolbarPanel::WxTreeScrolled::WxTreeScrolled(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr)
	: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 800))
{
	SetScrollbars(0, 1, 0, 10, 0, 0);

	auto sizer = new wxBoxSizer(wxVERTICAL);
	m_tree = new ee3::WxSkeletalTreeCtrl(this, sub_mgr);
	sizer->Add(m_tree, 1, wxEXPAND);
	SetSizer(sizer);
}

}
}