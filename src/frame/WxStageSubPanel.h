#pragma once

#include <wx/panel.h>

namespace eone
{

class WxStageSubPanel : public wxPanel
{
public:
	WxStageSubPanel(wxWindow* parent);

    void AddPagePanel(wxWindow* panel, int orient, int proportion = 0);

}; // WxStageSubPanel

}