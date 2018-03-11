#pragma once

#include <wx/dialog.h>

namespace eone
{

class WxSettingsDialog : public wxDialog
{
public:
	WxSettingsDialog(wxWindow* parent);

private:
	void InitLayout();

	void OnChangeEditType(wxCommandEvent& event);

}; // WxSettingsDialog

}