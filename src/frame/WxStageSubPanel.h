#pragma once

#include <wx/panel.h>

namespace eone
{

class WxStageSubPanel : public wxPanel
{
public:
	WxStageSubPanel(wxWindow* parent);

    wxPanel* SetPagePanel(int page_type, std::function<wxPanel*(wxPanel* parent)> page_creator, int orient);

private:
    int m_page_type = -1;

    wxPanel* m_panel = nullptr;

}; // WxStageSubPanel

}