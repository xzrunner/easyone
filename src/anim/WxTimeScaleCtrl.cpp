#include "anim/WxTimeScaleCtrl.h"
#include "anim/config.h"
#include "anim/MessageID.h"
#include "anim/LayerHelper.h"
#include "anim/MessageHelper.h"

#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>

#include <wx/dcbuffer.h>

namespace
{

static const int DIVISION_HEIGHT = 4;
static const int TEXT_Y = 4;

}

namespace eone
{
namespace anim
{

BEGIN_EVENT_TABLE(WxTimeScaleCtrl, wxPanel)
	EVT_PAINT(WxTimeScaleCtrl::OnPaint)
	EVT_SIZE(WxTimeScaleCtrl::OnSize)
	EVT_MOUSE_EVENTS(WxTimeScaleCtrl::OnMouse)
END_EVENT_TABLE()

WxTimeScaleCtrl::WxTimeScaleCtrl(wxWindow* parent, const n2::CompAnim& canim,
	                             const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1, FRAME_GRID_HEIGHT))
	, m_canim(canim)
	, m_sub_mgr(sub_mgr)
	, m_frame_idx(0)
{
	sub_mgr->RegisterObserver(MSG_SET_CURR_FRAME, this);
}

void WxTimeScaleCtrl::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case MSG_SET_CURR_FRAME:
		OnSetCurrFrame(variants);
		break;
	}
}

void WxTimeScaleCtrl::OnSize(wxSizeEvent& event)
{
	Refresh(false);
}

void WxTimeScaleCtrl::OnPaint(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);

	// background
	dc.SetPen(wxPen(MEDIUM_GRAY));
	dc.SetBrush(wxBrush(MEDIUM_GRAY));
	dc.DrawRectangle(GetSize());

	// curr pos
	if (m_frame_idx != -1)
	{
		int curr_pos = m_frame_idx;
		dc.SetPen(wxPen(DARK_RED));
		dc.SetBrush(wxBrush(MEDIUM_RED));
		dc.DrawRectangle(FRAME_GRID_WIDTH * curr_pos, 2, FRAME_GRID_WIDTH + 1, FRAME_GRID_HEIGHT - 2);
		dc.DrawLine(FRAME_GRID_WIDTH * (curr_pos + 0.5f), FRAME_GRID_HEIGHT, FRAME_GRID_WIDTH * (curr_pos + 0.5f), 100);
		if (curr_pos % 5 != 0)
		{
			wxSize size = dc.GetTextExtent(wxString::Format(wxT("%d"), curr_pos));
			dc.DrawText(wxString::Format(wxT("%d"), curr_pos),
				FRAME_GRID_WIDTH * (curr_pos + 0.5f) - size.GetWidth() / 2, TEXT_Y);
		}
	}

	// scale
	dc.SetPen(wxPen(TEXT_COLOR));
	for (int i = 0; i <= MAX_FRAME_COUNT; ++i)
	{
		dc.DrawLine(FRAME_GRID_WIDTH * i, FRAME_GRID_HEIGHT, FRAME_GRID_WIDTH * i, FRAME_GRID_HEIGHT - DIVISION_HEIGHT);
		if (i % 5 == 0)
		{
			wxSize size = dc.GetTextExtent(wxString::Format(wxT("%d"), i));
			dc.DrawText(wxString::Format(wxT("%d"), i),
				FRAME_GRID_WIDTH * (i + 0.5f) - size.GetWidth() / 2, TEXT_Y);
		}
	}
}

void WxTimeScaleCtrl::OnMouse(wxMouseEvent& event)
{
	if (event.LeftDown() || event.Dragging())
	{
		int frame = (int)(event.GetX() / FRAME_GRID_WIDTH);
		frame = std::min(LayerHelper::GetMaxFrame(m_canim), frame);
		if (m_frame_idx != frame) 
		{
			m_frame_idx = frame;
			Refresh(false);

			MessageHelper::SetCurrFrame(*m_sub_mgr, -1, frame);
		}
	}
}

void WxTimeScaleCtrl::OnSetCurrFrame(const ee0::VariantSet& variants)
{
	int layer, frame;

	auto var_layer = variants.GetVariant("layer");
	GD_ASSERT(var_layer.m_type == ee0::VT_INT, "err layer");
	layer = var_layer.m_val.l;

	auto var_frame = variants.GetVariant("frame");
	GD_ASSERT(var_frame.m_type == ee0::VT_INT, "err frame");
	frame = var_frame.m_val.l;

	if (layer == -1 && frame == -1) 
	{
		m_frame_idx = -1;
	}
	else 
	{
		int max_frame = LayerHelper::GetMaxFrame(m_canim, layer);
		if (max_frame >= 0) {
			m_frame_idx = std::min(max_frame, frame);
		}
	}
	Refresh(false);
}

}
}