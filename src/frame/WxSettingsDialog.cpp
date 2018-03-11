#include "frame/WxSettingsDialog.h"

#include <ee0/SettingCfg.h>

#include <guard/check.h>

#include <wx/radiobox.h>
#include <wx/sizer.h>

namespace eone
{

WxSettingsDialog::WxSettingsDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, "setting")
{
	InitLayout();
}

void WxSettingsDialog::InitLayout()
{
	wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxArrayString choices;
	choices.Add("shared");
	choices.Add("unique");
	auto radio = new wxRadioBox(this, wxID_ANY, "edit_type", wxDefaultPosition, wxDefaultSize, choices, 1, wxRA_SPECIFY_COLS);
	Connect(radio->GetId(), wxEVT_COMMAND_RADIOBOX_SELECTED, 
		wxCommandEventHandler(WxSettingsDialog::OnChangeEditType));
	sizer->Add(radio);

	SetSizer(sizer);
	sizer->Layout();
}

void WxSettingsDialog::OnChangeEditType(wxCommandEvent& event)
{
	int sel = event.GetSelection();
	switch (sel)
	{
	case 0:
		ee0::SettingCfg::Instance()->SetEditOpType(ee0::EditOpType::EDIT_SHARED);
		break;
	case 1:
		ee0::SettingCfg::Instance()->SetEditOpType(ee0::EditOpType::EDIT_UNIQUE);
		break;
	default:
		GD_REPORT_ASSERT("err type.");
	}
}

}