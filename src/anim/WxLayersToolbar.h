#pragma once

#include <wx/panel.h>

namespace eone
{
namespace anim
{

class WxLayersToolbar : public wxPanel
{
public:
	WxLayersToolbar(wxWindow* parent);

private:
	void InitLayout();

	void OnAddLayer(wxCommandEvent& event);
	void OnDelLayer(wxCommandEvent& event);

}; // WxLayersToolbar

}
}