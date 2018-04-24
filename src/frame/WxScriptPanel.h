#pragma once

#include <wx/panel.h>

namespace eone
{

class WxScriptPanel : public wxPanel
{
public:
	WxScriptPanel(wxWindow* parent);

private:
	void InitLayout();

}; // WxScriptPanel

}