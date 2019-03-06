#pragma once

#include <wx/panel.h>

namespace eone
{

class WxStageSubPanel : public wxPanel
{
public:
	WxStageSubPanel(wxWindow* parent);

    wxPanel* AddPagePanel(std::function<wxPanel*(wxPanel* parent)> page_creator, int orient);

}; // WxStageSubPanel

}