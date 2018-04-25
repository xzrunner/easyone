#pragma once

#include <wx/panel.h>

namespace ee0 { class WxCodeCtrl; }

class wxTextCtrl;

namespace eone
{

class WxScriptPanel : public wxPanel
{
public:
	WxScriptPanel(wxWindow* parent);

private:
	void InitLayout();

	void OnRunPress(wxCommandEvent& event);

private:
	ee0::WxCodeCtrl* m_input;

	wxTextCtrl* m_output;

}; // WxScriptPanel

}