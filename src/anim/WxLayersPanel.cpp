#include "anim/WxLayersPanel.h"
#include "anim/config.h"

#include <guard/check.h>
#include <node2/CompAnim.h>

#include <wx/dcclient.h>

namespace
{

const int DRAG_FLAG_SIZE = 3;
const int DRAG_FLAG_RADIUS = 4;

const int FLAG_RADIUS = 8;
const int FLAG_EDITABLE_X = 80;
const int FLAG_VISIBLE_X = 120;

}

namespace eone
{
namespace anim
{

BEGIN_EVENT_TABLE(WxLayersPanel, wxScrolledWindow)
	EVT_PAINT(WxLayersPanel::OnPaint)
	EVT_SIZE(WxLayersPanel::OnSize)
	EVT_MOUSE_EVENTS(WxLayersPanel::OnMouse)
END_EVENT_TABLE()

WxLayersPanel::WxLayersPanel(wxWindow* parent, const n2::CompAnim& canim)
	: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 300))
	, m_canim(canim)
	, m_drag_flag_line(-1)
	, m_curr_layer(-1)
{
	SetScrollbars(1, 1, 20, 10, 0, 0);
}

void WxLayersPanel::OnSize(wxSizeEvent& event)
{

}

void WxLayersPanel::OnPaint(wxPaintEvent& event)
{
//	GD_ASSERT(m_curr_layer >= 0, "err layer");

	auto& layers = m_canim.GetAllLayers();
	GD_ASSERT(!layers.empty(), "no data");

	int size = layers.size();
	const float width = GetSize().x;

	wxPaintDC dc(this);

	// background
	dc.SetPen(wxPen(LIGHT_GRAY));
	dc.SetBrush(wxBrush(LIGHT_GRAY));
	dc.DrawRectangle(GetSize());

	// white grids
	dc.SetPen(*wxWHITE_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.DrawRectangle(0, 0, width, FRAME_GRID_HEIGHT * size);

	// grid lines
	dc.SetPen(wxPen(MEDIUM_GRAY));
	for (int i = 0; i <= size; ++i)
	{
		float y = FRAME_GRID_HEIGHT * i;
		dc.DrawLine(0, y, width, y);
	}

	// selected
	if (m_curr_layer >= 0)
	{
		GD_ASSERT(static_cast<size_t>(m_curr_layer) < layers.size(), "err layer");
		int idx = size - m_curr_layer - 1;
		dc.SetPen(wxPen(MEDIUM_BLUE));
		dc.SetBrush(wxBrush(MEDIUM_BLUE));
		dc.DrawRectangle(0, FRAME_GRID_HEIGHT * idx, width, FRAME_GRID_HEIGHT);
	}

	// drag flag
	if (m_drag_flag_line != -1)
	{
		float y = FRAME_GRID_HEIGHT * m_drag_flag_line;
		dc.SetPen(wxPen(wxColour(0, 0, 0), DRAG_FLAG_SIZE));
		dc.SetBrush(*wxWHITE_BRUSH);
		dc.DrawCircle(FRAME_GRID_HEIGHT, y, DRAG_FLAG_RADIUS);
		dc.DrawLine(FRAME_GRID_HEIGHT + DRAG_FLAG_RADIUS, y, GetSize().x, y);
	}

	// layers' name
	dc.SetPen(wxPen(TEXT_COLOR));
	for (int i = 0; i < size; ++i)
	{
		size_t idx = size - i - 1;
		auto& layer = layers[idx];
//		dc.DrawText(layer->GetName(), 5, FRAME_GRID_HEIGHT * i);
		dc.DrawText("testzz", 5, FRAME_GRID_HEIGHT * i);

		dc.SetPen(*wxBLACK_PEN);
		//if (layer->IsEditable()) {
		//	dc.SetBrush(*wxBLACK_BRUSH);
		//} else {
		//	dc.SetBrush(*wxWHITE_BRUSH);
		//}
		dc.DrawRectangle(FLAG_EDITABLE_X, FRAME_GRID_HEIGHT * i + (FRAME_GRID_HEIGHT - FLAG_RADIUS*2) * 0.5, FLAG_RADIUS*2, FLAG_RADIUS*2);

		//if (layer->IsVisible()) {
		//	dc.SetBrush(*wxBLACK_BRUSH);
		//} else {
		//	dc.SetBrush(*wxWHITE_BRUSH);
		//}
		dc.DrawCircle(FLAG_VISIBLE_X, FRAME_GRID_HEIGHT * i + FRAME_GRID_HEIGHT * 0.5f, FLAG_RADIUS);
	}
}

void WxLayersPanel::OnMouse(wxMouseEvent& event)
{

}

}
}