#include "anim/WxTimeScaleCtrl.h"
#include "anim/config.h"
#include "anim/MessageID.h"
#include "anim/AnimHelper.h"
#include "anim/MessageHelper.h"

#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <node2/CompAnim.h>

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
	                             const ee0::SubjectMgrPtr& tl_sub_mgr)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1, FRAME_GRID_HEIGHT))
	, m_canim(canim)
	, m_tl_sub_mgr(tl_sub_mgr)
	, m_frame_idx(0)
	, m_start_x(0)
{
	tl_sub_mgr->RegisterObserver(MSG_SET_CURR_FRAME, this);
	tl_sub_mgr->RegisterObserver(MSG_WND_SCROLL, this);
}

void WxTimeScaleCtrl::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case MSG_SET_CURR_FRAME:
		OnSetCurrFrame(variants);
		break;
	case MSG_WND_SCROLL:
		OnWndScroll(variants);
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
		dc.DrawRectangle(FRAME_GRID_WIDTH * curr_pos - m_start_x, 2, FRAME_GRID_WIDTH + 1, FRAME_GRID_HEIGHT - 2);
		dc.DrawLine(FRAME_GRID_WIDTH * (curr_pos + 0.5f) - m_start_x, FRAME_GRID_HEIGHT, FRAME_GRID_WIDTH * (curr_pos + 0.5f) - m_start_x, 100);
		if (curr_pos % 5 != 0)
		{
			wxSize size = dc.GetTextExtent(wxString::Format(wxT("%d"), curr_pos));
			float x = FRAME_GRID_WIDTH * (curr_pos + 0.5f) - size.GetWidth() / 2;
			dc.DrawText(wxString::Format(wxT("%d"), curr_pos), x - m_start_x, TEXT_Y);
		}
	}

	// scale
	dc.SetPen(wxPen(TEXT_COLOR));
	for (int i = 0; i <= MAX_FRAME_COUNT; ++i)
	{
		dc.DrawLine(FRAME_GRID_WIDTH * i - m_start_x, FRAME_GRID_HEIGHT, FRAME_GRID_WIDTH * i - m_start_x, FRAME_GRID_HEIGHT - DIVISION_HEIGHT);
		if (i % 5 == 0)
		{
			wxSize size = dc.GetTextExtent(wxString::Format(wxT("%d"), i));
			float x = FRAME_GRID_WIDTH * (i + 0.5f) - size.GetWidth() / 2;
			dc.DrawText(wxString::Format(wxT("%d"), i), x - m_start_x, TEXT_Y);
		}
	}
}

void WxTimeScaleCtrl::OnMouse(wxMouseEvent& event)
{
	if (event.LeftDown() || event.Dragging())
	{
		int x = event.GetX() + m_start_x;
		int frame = (int)(x / FRAME_GRID_WIDTH);
		frame = std::min(AnimHelper::GetMaxFrame(m_canim), frame);
		if (m_frame_idx != frame) 
		{
			m_frame_idx = frame;
			Refresh(false);

			MessageHelper::SetCurrFrame(*m_tl_sub_mgr, -1, frame);
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
		int max_frame = AnimHelper::GetMaxFrame(m_canim, layer);
		if (max_frame >= 0) {
			m_frame_idx = std::min(max_frame, frame);
		}
	}
	Refresh(false);

	// update canim
	// todo: should not be here
	const_cast<n2::CompAnim&>(m_canim).SetCurrFrameIdx(m_frame_idx);
}

void WxTimeScaleCtrl::OnWndScroll(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("x");
	GD_ASSERT(var.m_type == ee0::VT_LONG, "err val");
	m_start_x = var.m_val.l;
	Refresh(false);
}

}
}