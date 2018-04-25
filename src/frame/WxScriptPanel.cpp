#include "frame/WxScriptPanel.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStagePage.h"

#include <ee0/WxCodeCtrl.h>
#include <ee0/EditPanelImpl.h>

#include <moon/ScriptHelper.h>
#include <moon/Context.h>
#include <moon/typedef.h>

#include <wx/sizer.h>
#include <wx/button.h>

namespace eone
{

WxScriptPanel::WxScriptPanel(wxWindow* parent)
	: wxPanel(parent, wxID_ANY)
{
	InitLayout();
}

void WxScriptPanel::InitLayout()
{
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(m_input = new ee0::WxCodeCtrl(this, "input"), 3, wxEXPAND);
//	m_input->InitializePrefs("Lua");

	sizer->Add(m_output = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_READONLY), 1, wxEXPAND);

	auto btn = new wxButton(this, wxID_ANY, "Run");
	Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(WxScriptPanel::OnRunPress));
	sizer->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);

	SetSizer(sizer);
}

void WxScriptPanel::OnRunPress(wxCommandEvent& event)
{
	auto page = Blackboard::Instance()->GetStagePanel()->GetCurrentStagePage();
	auto L = page->GetMoonCtx()->GetState();
	auto err_msg = moon::ScriptHelper::DoString(L, m_input->GetText().c_str());
	m_output->SetValue(err_msg);
}

}