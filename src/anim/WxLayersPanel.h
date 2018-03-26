#pragma once

#include <wx/scrolwin.h>

namespace n2 { class CompAnim; }

namespace eone
{
namespace anim
{

class WxLayersPanel : public wxScrolledWindow
{
public:
	WxLayersPanel(wxWindow* parent, const n2::CompAnim& canim);

private:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnMouse(wxMouseEvent& event);

private:
	const n2::CompAnim& m_canim;

	int m_drag_flag_line;

	int m_curr_layer;

	DECLARE_EVENT_TABLE()

}; // WxLayersPanel

}
}