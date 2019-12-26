#include "frame/WxStageSubPanel.h"

#include <wx/sizer.h>

namespace eone
{

WxStageSubPanel::WxStageSubPanel(wxWindow* parent)
	: wxPanel(parent)
{
}

void WxStageSubPanel::AddPagePanel(wxWindow* panel, int orient)
{
    bool new_sizer = false;

    auto sizer = GetSizer();
	if (!sizer) {
        new_sizer = true;
		sizer = new wxBoxSizer(orient);
	}

    if (panel) {
        sizer->Add(panel, 1, wxEXPAND);
//        sizer->Add(panel);
    }

    if (new_sizer) {
        SetSizer(sizer);
    }
}

}