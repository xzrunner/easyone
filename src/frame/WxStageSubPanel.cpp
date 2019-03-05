#include "frame/WxStageSubPanel.h"

#include <wx/sizer.h>

namespace eone
{

WxStageSubPanel::WxStageSubPanel(wxWindow* parent)
	: wxPanel(parent)
{
}

wxPanel* WxStageSubPanel::SetPagePanel(int page_type, std::function<wxPanel*(wxPanel* parent)> page_creator, int orient)
{
    if (m_page_type == page_type) {
        return m_panel;
    }

    m_page_type = page_type;

    bool new_sizer = false;

    auto sizer = GetSizer();
	if (sizer) {
		sizer->Clear(true);
	} else {
        new_sizer = true;
		sizer = new wxBoxSizer(orient);
	}

    m_panel = page_creator(this);
    if (m_panel) {
        sizer->Add(m_panel, 0, wxEXPAND);
    }

    if (new_sizer) {
        SetSizer(sizer);
    }

    return m_panel;
}

}