#include "anim/WxTimeStagePanel.h"
#include "anim/config.h"
#include "anim/MessageID.h"
#include "anim/MessageHelper.h"
#include "anim/LayerHelper.h"

#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <node2/CompAnim.h>

#include <wx/dcbuffer.h>

namespace
{

const int KEY_FRAME_CIRCLE_Y_OFFSET = eone::anim::FRAME_GRID_WIDTH * 0.5f + 1;
const int KEY_FRAME_CIRCLE_RADIUS   = eone::anim::FRAME_GRID_WIDTH * 0.4f;
const int KEY_FRAME_RECT_Y_OFFSET   = KEY_FRAME_CIRCLE_Y_OFFSET + 1;
const int FRAME_END_RECT_WIDTH      = KEY_FRAME_CIRCLE_RADIUS * 2;
const int FRAME_END_RECT_HEIGHT     = KEY_FRAME_CIRCLE_RADIUS * 3;

}

namespace eone
{
namespace anim
{

BEGIN_EVENT_TABLE(WxTimeStagePanel, wxScrolledWindow)
	EVT_PAINT(WxTimeStagePanel::OnPaint)
	EVT_SIZE(WxTimeStagePanel::OnSize)
	EVT_MOUSE_EVENTS(WxTimeStagePanel::OnMouse)
	EVT_KEY_DOWN(WxTimeStagePanel::OnKeyDown)
	EVT_KEY_UP(WxTimeStagePanel::OnKeyUp)
END_EVENT_TABLE()

WxTimeStagePanel::WxTimeStagePanel(wxWindow* parent, const n2::CompAnim& canim,
	                               const ee0::SubjectMgrPtr& sub_mgr)
	: wxScrolledWindow(parent/*, wxID_ANY, wxDefaultPosition, wxSize(100, 500)*/)
	, m_canim(canim)
	, m_sub_mgr(sub_mgr)
	, m_editop(canim, sub_mgr)
{
	m_layer_idx = m_frame_idx = m_valid_frame_idx = -1;
	m_col_min = m_col_max = -1;

	SetScrollbars(1, 1, 20, 10, 0, 0);

	m_sub_mgr->RegisterObserver(MSG_SET_CURR_FRAME, this);
	m_sub_mgr->RegisterObserver(MSG_SET_SELECTED_REGION, this);
}

void WxTimeStagePanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case MSG_SET_CURR_FRAME:
		OnSetCurrFrame(variants);
		break;
	case MSG_SET_SELECTED_REGION:
		OnSetSelectedRegion(variants);
		break;
	}
}

void WxTimeStagePanel::OnSize(wxSizeEvent& event)
{
	Refresh(false);
}

void WxTimeStagePanel::OnPaint(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);

	DrawBackground(dc);
	DrawLayersDataBg(dc);
	DrawSelected(dc);
	DrawLayersDataFlag(dc);
	DrawCurrPosFlag(dc);
}

void WxTimeStagePanel::OnMouse(wxMouseEvent& event)
{
	int row = event.GetY() / FRAME_GRID_HEIGHT,
		col = event.GetX() / FRAME_GRID_WIDTH;

	if (event.RightDown()) {
		m_editop.OnMouseLeftDown(row, col);
//		MousePopupMenu(event.GetX(), event.GetY());
	} else if (event.LeftDown()) {
		m_editop.OnMouseLeftDown(row, col);
	} else if (event.Dragging()) {
		m_editop.OnMouseDragging(row, col);
	} else if (event.Moving()) {
		SetFocus();
	} else if (event.LeftDClick()) {
		m_editop.OnMouseLeftClick(row, col);
	}
}

void WxTimeStagePanel::OnKeyDown(wxKeyEvent& event)
{

}

void WxTimeStagePanel::OnKeyUp(wxKeyEvent& event)
{

}

void WxTimeStagePanel::DrawBackground(wxBufferedPaintDC& dc)
{
	// background
	dc.SetPen(wxPen(LIGHT_GRAY));
	dc.SetBrush(wxBrush(LIGHT_GRAY));
	dc.DrawRectangle(GetSize());

	auto& layers = m_canim.GetAllLayers();
	const size_t size = layers.size();
	const float width = FRAME_GRID_WIDTH * MAX_FRAME_COUNT,
		        height = FRAME_GRID_HEIGHT * size;

	// white grids
	dc.SetPen(*wxWHITE_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);
 	dc.DrawRectangle(0, 0, width, height);

	// 5 times gray grids
	dc.SetPen(wxPen(LIGHT_GRAY));
	dc.SetBrush(wxBrush(LIGHT_GRAY));
	for (size_t i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		if (i % 5 == 0)
		{
			const float x = FRAME_GRID_WIDTH * i;
			dc.DrawRectangle(x, 0, FRAME_GRID_WIDTH, height);
		}
	}

	// grid lines
	dc.SetPen(wxPen(MEDIUM_GRAY));
	for (size_t i = 0; i <= size; ++i)
	{
		const float y = FRAME_GRID_HEIGHT * i;
		dc.DrawLine(0, y, width, y);
	}
	for (size_t i = 0; i <= MAX_FRAME_COUNT; ++i)
	{
		const float x = FRAME_GRID_WIDTH * i;
		dc.DrawLine(x, 0, x, height);
	}
}

void WxTimeStagePanel::DrawLayersDataBg(wxBufferedPaintDC& dc)
{
	auto& layers = m_canim.GetAllLayers();
	for (size_t i = 0, n = layers.size(); i < n; ++i)
	{
		size_t idx = n - i - 1;
		auto& frames = layers[idx]->frames;
		// during
		for (auto itr = frames.begin(); itr != frames.end(); ++itr)
		{
			auto next = itr;
			++next;
			float x = FRAME_GRID_WIDTH * (*itr)->index,
				  y = FRAME_GRID_HEIGHT * i;
			float width = FRAME_GRID_WIDTH + 1,
				  height = FRAME_GRID_HEIGHT + 1;
			if (next != frames.end()) {
				width = std::max(FRAME_GRID_WIDTH * ((*next)->index - (*itr)->index), FRAME_GRID_WIDTH) + 1;
			}
			dc.SetPen(*wxBLACK_PEN);
//			dc.SetBrush(*wxWHITE_BRUSH);
			dc.SetBrush(wxColour(255, 255, 255, 128));
			dc.DrawRectangle(x, y, width, height);
		}
		// Classic Tween
		for (auto itr = frames.begin(); itr != frames.end(); ++itr)
		{
			if ((*itr)->tween)
			{
				auto next = itr;
				++next;
				if (next != frames.end())
				{
					float x = FRAME_GRID_WIDTH * (*itr)->index,
						  y = FRAME_GRID_HEIGHT * i;
					float width = FRAME_GRID_WIDTH * ((*next)->index - (*itr)->index) + 1,
						  height = FRAME_GRID_HEIGHT + 1;

					dc.SetPen(*wxBLACK_PEN);
					dc.SetBrush(wxBrush(CLASSIC_TWEEN_COLOR));
					dc.DrawRectangle(x, y, width, height);
				}
			}
		}
	}
}

void WxTimeStagePanel::DrawLayersDataFlag(wxBufferedPaintDC& dc)
{
	auto& layers = m_canim.GetAllLayers();
	for (size_t i = 0, n = layers.size(); i < n; ++i)
	{
		size_t idx = n - i - 1;
		auto& frames = layers[idx]->frames;
		// key frame start (circle)
		for (auto& frame : frames)
		{
			float x = FRAME_GRID_WIDTH * (frame->index + 0.5f),
				  y = FRAME_GRID_HEIGHT * (i + 1) - KEY_FRAME_CIRCLE_Y_OFFSET;
			dc.SetPen(*wxBLACK_PEN);
			if (frame->nodes.empty()) {
				dc.SetBrush(*wxWHITE_BRUSH);
			} else {
				dc.SetBrush(*wxBLACK_BRUSH);
			}
			dc.DrawCircle(x, y, KEY_FRAME_CIRCLE_RADIUS);
		}
		// key frame end (rect)
		for (auto itr = frames.begin(); itr != frames.end(); ++itr)
		{
			if (itr == frames.begin()) {
				continue;
			}

			auto prev = itr;
			--prev;
			if ((*prev)->index == (*itr)->index - 1) {
				continue;
			}

			float x = FRAME_GRID_WIDTH * ((*itr)->index - 0.5f) - FRAME_END_RECT_WIDTH * 0.5f,
				  y = FRAME_GRID_HEIGHT * (i + 1) - KEY_FRAME_RECT_Y_OFFSET - FRAME_END_RECT_HEIGHT * 0.5f;
			dc.SetPen(*wxBLACK_PEN);
			dc.SetBrush(*wxWHITE_BRUSH);
			dc.DrawRectangle(x, y, FRAME_END_RECT_WIDTH, FRAME_END_RECT_HEIGHT);
		}
	}
}

void WxTimeStagePanel::DrawCurrPosFlag(wxBufferedPaintDC& dc)
{
	const float x = FRAME_GRID_WIDTH * (m_valid_frame_idx + 0.5f);
	dc.SetPen(DARK_RED);
	dc.DrawLine(x, 0, x, FRAME_GRID_HEIGHT * m_canim.GetAllLayers().size());
}

void WxTimeStagePanel::DrawSelected(wxBufferedPaintDC& dc)
{
	if (m_layer_idx == -1) {
		return;
	}
	auto& layers = m_canim.GetAllLayers();
	int row = layers.size() - 1 - m_layer_idx;
	if (m_col_min != -1 && m_col_max != -1) 
	{
		dc.SetPen(wxPen(DARK_BLUE));
		dc.SetBrush(wxBrush(DARK_BLUE));
		dc.DrawRectangle(
			FRAME_GRID_WIDTH * m_col_min, 
			FRAME_GRID_HEIGHT * row, 
			FRAME_GRID_WIDTH * (m_col_max - m_col_min + 1), 
			FRAME_GRID_HEIGHT
		);
	} 
	else 
	{
		dc.SetPen(wxPen(DARK_BLUE));
		dc.SetBrush(wxBrush(DARK_BLUE));
		dc.DrawRectangle(
			FRAME_GRID_WIDTH * m_frame_idx, 
			FRAME_GRID_HEIGHT * row, 
			FRAME_GRID_WIDTH, 
			FRAME_GRID_HEIGHT
		);
	}
}

void WxTimeStagePanel::OnSetCurrFrame(const ee0::VariantSet& variants)
{
	int layer, frame;
	
	auto var_layer = variants.GetVariant("layer");
	GD_ASSERT(var_layer.m_type == ee0::VT_INT, "err layer");
	layer = var_layer.m_val.l;

	auto var_frame = variants.GetVariant("frame");
	GD_ASSERT(var_frame.m_type == ee0::VT_INT, "err frame");
	frame = var_frame.m_val.l;

	bool dirty = false;
	m_col_min = m_col_max = -1;
	if (layer == -1 && frame == -1) 
	{
		m_layer_idx = m_frame_idx = m_valid_frame_idx = -1;
		m_col_min = m_col_max = -1;
		dirty = true;
	} 
	else 
	{
		if (layer != -1 && layer != m_layer_idx) 
		{
			m_layer_idx = layer;
			dirty = true;
		}
		if (frame != m_frame_idx) 
		{
			if (frame != -1) {
				m_frame_idx = frame;
			}
 			dirty = true;
		}
		if (m_layer_idx >= 0)
		{
			int max_frame = LayerHelper::GetMaxFrame(m_canim, m_layer_idx);
			if (max_frame >= 0) 
			{
				int valid_frame_idx = std::min(max_frame, m_frame_idx);
				if (valid_frame_idx != m_valid_frame_idx) {
					m_valid_frame_idx = valid_frame_idx;
					dirty = true;
				}
			}
		}
	}

	if (dirty) {
		Refresh(false);
	}
}

void WxTimeStagePanel::OnSetSelectedRegion(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("col");
	GD_ASSERT(var.m_type == ee0::VT_INT, "err col");
	int col = var.m_val.l;

	m_col_min = m_col_max = -1;

	if (m_layer_idx >= 0)
	{
		int max_frame = LayerHelper::GetMaxFrame(m_canim, m_layer_idx);
		if (max_frame >= 0) 
		{
			m_col_min = std::min(std::min(m_frame_idx, col), max_frame);
			m_col_max = std::min(std::max(m_frame_idx, col), max_frame);
			if (m_col_min == m_col_max) {
				MessageHelper::SetCurrFrame(*m_sub_mgr, -1, m_col_min);
			}
			Refresh(false);
		}
	}
}

}
}