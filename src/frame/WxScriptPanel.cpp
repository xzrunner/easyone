#include "frame/WxScriptPanel.h"

#include <wx/sizer.h>

namespace eone
{

WxScriptPanel::WxScriptPanel(wxWindow* parent)
	: wxPanel(parent, wxID_ANY)
{
	InitLayout();
}

void WxScriptPanel::InitLayout()
{
	wxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);

	

	SetSizer(top_sizer);
}

}