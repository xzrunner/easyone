#include "anim/WxLayersPanel.h"
#include "anim/config.h"
#include "anim/MessageID.h"
#include "anim/MsgHelper.h"

#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <node2/CompAnim.h>
#include <anim/Layer.h>
#include <anim/KeyFrame.h>

#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>

namespace
{

const int DRAG_FLAG_SIZE = 3;
const int DRAG_FLAG_RADIUS = 4;

const int FLAG_RADIUS = 8;
const int FLAG_EDITABLE_X = 80;
const int FLAG_VISIBLE_X = 120;

class SetValueDialog : public wxDialog
{
public:
	SetValueDialog(wxWindow* parent, const std::string& title,
		const std::string& val, const wxPoint& pos)
		: wxDialog(parent, wxID_ANY, title, pos)
	{
		InitLayout(val);
		SetPosition(wxPoint(pos.x, pos.y - GetSize().y));
	}

	std::string GetText() const
	{
		return m_text_ctrl->GetLineText(0).ToStdString();
	}

private:
	void InitLayout(const std::string& val)
	{
		wxBoxSizer* top_sz = new wxBoxSizer(wxVERTICAL);

		m_text_ctrl = new wxTextCtrl(this, wxID_ANY, val);
		top_sz->Add(m_text_ctrl, 0, wxALIGN_CENTRE | wxALL, 5);

		wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
		btnSizer->Add(new wxButton(this, wxID_OK), wxALL, 5);
		btnSizer->Add(new wxButton(this, wxID_CANCEL), wxALL, 5);
		top_sz->Add(btnSizer, 0, wxALL, 5);

		SetSizer(top_sz);
		top_sz->Fit(this);
	}

private:
	wxTextCtrl* m_text_ctrl;

}; // SetValueDialog

const uint32_t MESSAGES[] =
{
	eone::anim::MSG_SET_CURR_FRAME,
};

}

namespace eone
{
namespace anim
{

BEGIN_EVENT_TABLE(WxLayersPanel, wxPanel)
	EVT_PAINT(WxLayersPanel::OnPaint)
	EVT_ERASE_BACKGROUND(WxLayersPanel::OnEraseBackground)
	EVT_SIZE(WxLayersPanel::OnSize)
	EVT_MOUSE_EVENTS(WxLayersPanel::OnMouse)
END_EVENT_TABLE()

WxLayersPanel::WxLayersPanel(wxWindow* parent, const n2::CompAnim& canim,
	                         const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1, 999))
	, m_canim(canim)
	, m_sub_mgr(sub_mgr)
	, m_drag_flag_line(-1)
	, m_curr_layer(-1)
	, m_is_drag_open(false)
	, m_xpress(0)
	, m_ypress(0)
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->RegisterObserver(msg, this);
	}
}

WxLayersPanel::~WxLayersPanel()
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->UnregisterObserver(msg, this);
	}
}

void WxLayersPanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case MSG_SET_CURR_FRAME:
		OnSetCurrFrame(variants);
		break;
	}
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
		dc.DrawText(layer->GetName(), 5, FRAME_GRID_HEIGHT * i);

		dc.SetPen(*wxBLACK_PEN);
		if (layer->IsEditable()) {
			dc.SetBrush(*wxBLACK_BRUSH);
		} else {
			dc.SetBrush(*wxWHITE_BRUSH);
		}
		dc.DrawRectangle(FLAG_EDITABLE_X, FRAME_GRID_HEIGHT * i + (FRAME_GRID_HEIGHT - FLAG_RADIUS*2) * 0.5, FLAG_RADIUS*2, FLAG_RADIUS*2);

		if (layer->IsVisible()) {
			dc.SetBrush(*wxBLACK_BRUSH);
		} else {
			dc.SetBrush(*wxWHITE_BRUSH);
		}
		dc.DrawCircle(FLAG_VISIBLE_X, FRAME_GRID_HEIGHT * i + FRAME_GRID_HEIGHT * 0.5f, FLAG_RADIUS);
	}
}

void WxLayersPanel::OnEraseBackground(wxEraseEvent& event)
{
	event.Skip();
}

void WxLayersPanel::OnMouse(wxMouseEvent& event)
{
	auto& layers = m_canim.GetAllLayers();
	auto size = layers.size();
	if (event.LeftDown())
	{
		m_xpress = event.GetX();
		m_ypress = event.GetY();

		unsigned int screen_idx = event.GetY() / FRAME_GRID_HEIGHT;
		int layer = size - screen_idx - 1;
		if (layer >= 0) 
		{
			MsgHelper::SetCurrFrame(*m_sub_mgr, layer, -1);
			if (screen_idx < size) {
				m_is_drag_open = true;
			}
		}
	}
	else if (event.LeftUp())
	{
		if (m_xpress == event.GetX() && m_ypress == event.GetY())
		{
			unsigned int screen_idx = event.GetY() / FRAME_GRID_HEIGHT;
			int layer_idx = size - screen_idx - 1;
			if (layer_idx >= 0 && static_cast<size_t>(layer_idx) < layers.size())
			{
				int x = event.GetX();
				auto& layer = layers[layer_idx];
				if (layer && x > FLAG_EDITABLE_X && x < FLAG_EDITABLE_X + FLAG_RADIUS * 2)
				{
					layer->SetEditable(!layer->IsEditable());
					Refresh(true);
				}
				else if (layer && x > FLAG_VISIBLE_X - FLAG_RADIUS && x < FLAG_VISIBLE_X + FLAG_RADIUS)
				{
					layer->SetVisible(!layer->IsVisible());
					Refresh(true);
				}
			}
		}

		m_is_drag_open = false;
		if (m_drag_flag_line != -1) 
		{
			int from = m_curr_layer,
				to = size - m_drag_flag_line;
			if (to == from || to == from + 1)
				;
			else
			{
				if (to > from) --to;
				const_cast<n2::CompAnim&>(m_canim).SwapLayers(from, to);
				m_sub_mgr->NotifyObservers(MSG_REFRESH_ANIM_COMP);
				MsgHelper::SetCurrFrame(*m_sub_mgr, to, -1);
			}
		}
		m_drag_flag_line = -1;
	}
	else if (event.Dragging())
	{
		if (m_is_drag_open && size != 0)
		{
			int newDragLine = m_drag_flag_line;
			int y = event.GetY();
			if (y < 0) newDragLine = 0;
			else if (static_cast<size_t>(y) > FRAME_GRID_HEIGHT * size) newDragLine = size;
			else newDragLine = (float)y / FRAME_GRID_HEIGHT + 0.5f;
			if (newDragLine != m_drag_flag_line) 
			{
				m_drag_flag_line = newDragLine;
				Refresh(true);
			}
		}
	}
	else if (event.LeftDClick())
	{
		int screen_idx = event.GetY() / FRAME_GRID_HEIGHT;
		int layer_idx = size - screen_idx - 1;
		if (static_cast<size_t>(layer_idx) < size)
		{
			auto& layer = layers[layer_idx];
			wxPoint pos(GetScreenPosition() + wxPoint(event.GetX(), event.GetY()));
			SetValueDialog dlg(this, "Set layer's name", layer->GetName(), pos);
			if (dlg.ShowModal() == wxID_OK)
			{
				layer->SetName(dlg.GetText());
				Refresh(true);
			}
		}
	}
}

void WxLayersPanel::OnSetCurrFrame(const ee0::VariantSet& variants)
{
	int layer, frame;

	auto var_layer = variants.GetVariant("layer");
	GD_ASSERT(var_layer.m_type == ee0::VT_INT, "err layer");
	layer = var_layer.m_val.l;

	auto var_frame = variants.GetVariant("frame");
	GD_ASSERT(var_frame.m_type == ee0::VT_INT, "err frame");
	frame = var_frame.m_val.l;

	if (layer == -1 && frame == -1) {
		m_curr_layer = -1;
		Refresh(true);
	} else if (layer != -1 && layer != m_curr_layer) {
		m_curr_layer = layer;
		Refresh(true);
	}
}

}
}