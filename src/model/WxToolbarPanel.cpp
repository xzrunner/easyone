#include "model/WxToolbarPanel.h"
#include "model/WxStagePage.h"

#include <ee0/SubjectMgr.h>

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
	wxArrayString choices;
	choices.Add("Rotate Joint");
	choices.Add("Translate Joint");
	choices.Add("IK");
	m_edit_op = new wxRadioBox(this, wxID_ANY, "edit_op", wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);
	Connect(m_edit_op->GetId(), wxEVT_COMMAND_RADIOBOX_SELECTED,
		wxCommandEventHandler(WxToolbarPanel::OnChangeEditType));
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

}
}