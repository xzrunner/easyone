#include "frame/WxStageSubPanel.h"

#include <wx/sizer.h>

namespace eone
{

WxStageSubPanel::WxStageSubPanel(wxWindow* parent)
	: wxPanel(parent)
{
}

wxPanel* WxStageSubPanel::AddPagePanel(std::function<wxPanel*(wxPanel* parent)> page_creator, int orient)
{
    bool new_sizer = false;

    auto sizer = GetSizer();
	if (!sizer) {
        new_sizer = true;
		sizer = new wxBoxSizer(orient);
	}

    auto panel = page_creator(this);
    if (panel) {
        sizer->Add(panel, 1, wxEXPAND);
//        sizer->Add(panel);
    }

    if (new_sizer) {
        SetSizer(sizer);
    }

    return panel;
}

}