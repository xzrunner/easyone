#include "anim/WxTimeStagePanel.h"
#include "anim/config.h"
#include "anim/MessageID.h"
#include "anim/MessageHelper.h"
#include "anim/AnimHelper.h"

#include <ee0/VariantSet.h>
#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <node2/CompAnim.h>
#include <anim/KeyFrame.h>
#include <anim/Layer.h>

#include <wx/dcbuffer.h>
#include <wx/menu.h>

namespace
{

const int KEY_FRAME_CIRCLE_Y_OFFSET = eone::anim::FRAME_GRID_WIDTH * 0.5f + 1;
const int KEY_FRAME_CIRCLE_RADIUS   = eone::anim::FRAME_GRID_WIDTH * 0.4f;
const int KEY_FRAME_RECT_Y_OFFSET   = KEY_FRAME_CIRCLE_Y_OFFSET + 1;
const int FRAME_END_RECT_WIDTH      = KEY_FRAME_CIRCLE_RADIUS * 2;
const int FRAME_END_RECT_HEIGHT     = KEY_FRAME_CIRCLE_RADIUS * 3;

const uint32_t MESSAGES[] = 
{
	ee0::MSG_INSERT_SCENE_NODE,
	ee0::MSG_DELETE_SCENE_NODE,
	ee0::MSG_CLEAR_SCENE_NODE,
	ee0::MSG_REORDER_SCENE_NODE,

	ee0::MSG_UPDATE_NODES,

	eone::anim::MSG_SET_CURR_FRAME,
	eone::anim::MSG_SET_SELECTED_REGION,
};

}

namespace eone
{
namespace anim
{

BEGIN_EVENT_TABLE(WxTimeStagePanel, wxPanel)
	EVT_PAINT(WxTimeStagePanel::OnPaint)
	EVT_SIZE(WxTimeStagePanel::OnSize)
	EVT_MOUSE_EVENTS(WxTimeStagePanel::OnMouse)
	EVT_KEY_DOWN(WxTimeStagePanel::OnKeyDown)
	EVT_KEY_UP(WxTimeStagePanel::OnKeyUp)

	EVT_MENU(Menu_CreateClassicTween, WxTimeStagePanel::OnCreateClassicTween)
	EVT_MENU(Menu_DeleteClassicTween, WxTimeStagePanel::OnDeleteClassicTween)
	EVT_MENU(Menu_InsertFrame, WxTimeStagePanel::OnInsertFrame)
	EVT_MENU(Menu_DeleteFrame, WxTimeStagePanel::OnDeleteFrame)
	EVT_MENU(Menu_InsertKeyFrame, WxTimeStagePanel::OnInsertKeyFrame)
	EVT_MENU(Menu_DeleteKeyFrame, WxTimeStagePanel::OnDeleteKeyFrame)

	EVT_UPDATE_UI(Menu_CreateClassicTween, WxTimeStagePanel::OnUpdateCreateClassicTween)
	EVT_UPDATE_UI(Menu_DeleteClassicTween, WxTimeStagePanel::OnUpdateDeleteClassicTween)
	EVT_UPDATE_UI(Menu_InsertFrame, WxTimeStagePanel::OnUpdateInsertFrame)
	EVT_UPDATE_UI(Menu_DeleteFrame, WxTimeStagePanel::OnUpdateDeleteFrame)
	EVT_UPDATE_UI(Menu_InsertKeyFrame, WxTimeStagePanel::OnUpdateInsertKeyFrame)
	EVT_UPDATE_UI(Menu_DeleteKeyFrame, WxTimeStagePanel::OnUpdateDeleteKeyFrame)
END_EVENT_TABLE()

LanguageEntry WxTimeStagePanel::entries[] =
{
	{ "创建传统补间", "Create Classic Tween" },
	{ "删除传统补间", "Delete Classic Tween" },
	{ "插入帧", "Insert Frame" },
	{ "删除帧", "Delete Frame" },
	{ "插入关键帧", "Insert Key Frame" },
	{ "删除关键帧", "Delete Key Frame" }
};

WxTimeStagePanel::WxTimeStagePanel(wxWindow* parent, const n2::CompAnim& canim,
	                               const ee0::SubjectMgrPtr& sub_mgr)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(5000, 999))
	, m_canim(canim)
	, m_sub_mgr(sub_mgr)
	, m_editop(canim, sub_mgr)
{
	m_layer_idx = m_frame_idx = m_valid_frame_idx = -1;
	m_col_min = m_col_max = -1;

	for (auto& msg : MESSAGES) {
		m_sub_mgr->RegisterObserver(msg, this);
	}
}

WxTimeStagePanel::~WxTimeStagePanel()
{
	for (auto& msg : MESSAGES) {
		m_sub_mgr->UnregisterObserver(msg, this);
	}
}

void WxTimeStagePanel::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	bool dirty = false;
	switch (msg)
	{
	case ee0::MSG_INSERT_SCENE_NODE:
		dirty = InsertSceneNode(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
		dirty = DeleteSceneNode(variants);
		break;
	case ee0::MSG_CLEAR_SCENE_NODE:
		dirty = ClearSceneNode();
		break;
	case ee0::MSG_REORDER_SCENE_NODE:
		dirty = ReorderSceneNode(variants);
		break;

	case ee0::MSG_UPDATE_NODES:
		OnUpdateNode();
		break;

	case MSG_SET_CURR_FRAME:
		OnSetCurrFrame(variants);
		break;
	case MSG_SET_SELECTED_REGION:
		OnSetSelectedRegion(variants);
		break;
	}

	if (dirty) {
		Refresh(false);
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
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
		MousePopupMenu(event.GetX(), event.GetY());
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
		auto& frames = layers[idx]->GetAllKeyFrames();
		// during
		for (auto itr = frames.begin(); itr != frames.end(); ++itr)
		{
			auto next = itr;
			++next;
			float x = FRAME_GRID_WIDTH * (*itr)->GetFrameIdx(),
				  y = FRAME_GRID_HEIGHT * i;
			float width = FRAME_GRID_WIDTH + 1,
				  height = FRAME_GRID_HEIGHT + 1;
			if (next != frames.end()) {
				width = std::max(FRAME_GRID_WIDTH * ((*next)->GetFrameIdx() - (*itr)->GetFrameIdx()), FRAME_GRID_WIDTH) + 1;
			}
			dc.SetPen(*wxBLACK_PEN);
//			dc.SetBrush(*wxWHITE_BRUSH);
			dc.SetBrush(wxColour(255, 255, 255, 128));
			dc.DrawRectangle(x, y, width, height);
		}
		// Classic Tween
		for (auto itr = frames.begin(); itr != frames.end(); ++itr)
		{
			if ((*itr)->GetTween())
			{
				auto next = itr;
				++next;
				if (next != frames.end())
				{
					float x = FRAME_GRID_WIDTH * (*itr)->GetFrameIdx(),
						  y = FRAME_GRID_HEIGHT * i;
					float width = FRAME_GRID_WIDTH * ((*next)->GetFrameIdx() - (*itr)->GetFrameIdx()) + 1,
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
		auto& frames = layers[idx]->GetAllKeyFrames();
		// key frame start (circle)
		for (auto& frame : frames)
		{
			float x = FRAME_GRID_WIDTH * (frame->GetFrameIdx() + 0.5f),
				  y = FRAME_GRID_HEIGHT * (i + 1) - KEY_FRAME_CIRCLE_Y_OFFSET;
			dc.SetPen(*wxBLACK_PEN);
			if (frame->GetAllNodes().empty()) {
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
			if ((*prev)->GetFrameIdx() == (*itr)->GetFrameIdx() - 1) {
				continue;
			}

			float x = FRAME_GRID_WIDTH * ((*itr)->GetFrameIdx() - 0.5f) - FRAME_END_RECT_WIDTH * 0.5f,
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

bool WxTimeStagePanel::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx);
	if (!frame) {
		return false;
	}

	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	frame->AddNode(*node);

	return true;
}

bool WxTimeStagePanel::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx);
	if (!frame) {
		return false;
	}

	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	return frame->RemoveNode(*node);
}

bool WxTimeStagePanel::ClearSceneNode()
{
	return const_cast<n2::CompAnim&>(m_canim).RemoveAllLayers();
}

bool WxTimeStagePanel::ReorderSceneNode(const ee0::VariantSet& variants)
{
	auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx);
	if (!frame) {
		return false;
	}

	auto node_var = variants.GetVariant("node");
	GD_ASSERT(node_var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(node_var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto up_var = variants.GetVariant("up");
	GD_ASSERT(up_var.m_type == ee0::VT_BOOL, "no var in vars: up");
	bool up = up_var.m_val.bl;

	std::vector<n0::SceneNodePtr> all_nodes = frame->GetAllNodes();
	if (all_nodes.empty()) {
		return false;
	}

	int idx = -1;
	for (int i = 0, n = all_nodes.size(); i < n; ++i)
	{
		if (all_nodes[i] == *node) {
			idx = i;
			break;
		}
	}

	GD_ASSERT(idx >= 0, "not find");

	if (up && idx != all_nodes.size() - 1)
	{
		std::swap(all_nodes[idx], all_nodes[idx + 1]);
		frame->SetNodes(all_nodes);
		return true;
	}
	else if (!up && idx != 0)
	{
		std::swap(all_nodes[idx], all_nodes[idx - 1]);
		frame->SetNodes(all_nodes);
		return true;
	}

	return false;
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
		if (auto layer = AnimHelper::GetLayer(m_canim, m_layer_idx))
		{
			int max_frame = layer->GetMaxFrameIdx();
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

	if (auto layer = AnimHelper::GetLayer(m_canim, m_layer_idx))
	{
		int max_frame = layer->GetMaxFrameIdx();
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

void WxTimeStagePanel::MousePopupMenu(int x, int y)
{
	wxMenu menu;

	menu.Append(Menu_CreateClassicTween, entries[Menu_CreateClassicTween].text[currLanguage]);
	menu.Append(Menu_DeleteClassicTween, entries[Menu_DeleteClassicTween].text[currLanguage]);
	menu.AppendSeparator();
	menu.Append(Menu_InsertFrame, entries[Menu_InsertFrame].text[currLanguage]);
	menu.Append(Menu_DeleteFrame, entries[Menu_DeleteFrame].text[currLanguage]);
	menu.AppendSeparator();
	menu.Append(Menu_InsertKeyFrame, entries[Menu_InsertKeyFrame].text[currLanguage]);
	menu.Append(Menu_DeleteKeyFrame, entries[Menu_DeleteKeyFrame].text[currLanguage]);

	PopupMenu(&menu, x, y);	
}

void WxTimeStagePanel::OnCreateClassicTween(wxCommandEvent& event)
{
	if (auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx)) {
		frame->SetTween(true);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnDeleteClassicTween(wxCommandEvent& event)
{
	if (auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx)) {
		frame->SetTween(false);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnInsertFrame(wxCommandEvent& event)
{
	OnInsertFrame();
}

void WxTimeStagePanel::OnDeleteFrame(wxCommandEvent& event)
{
	OnDeleteFrame();
}

void WxTimeStagePanel::OnInsertKeyFrame(wxCommandEvent& event)
{
	if (auto layer = AnimHelper::GetLayer(m_canim, m_layer_idx)) {
		layer->InsertKeyFrame(m_frame_idx);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnDeleteKeyFrame(wxCommandEvent& event)
{
	if (auto layer = AnimHelper::GetLayer(m_canim, m_layer_idx)) {
		layer->RemoveKeyFrame(m_frame_idx);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnUpdateCreateClassicTween(wxUpdateUIEvent& event)
{
	if (auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx)) {
		event.Enable(!frame->GetTween());
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateDeleteClassicTween(wxUpdateUIEvent& event)
{
	if (auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx)) {
		event.Enable(frame->GetTween());
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateInsertFrame(wxUpdateUIEvent& event)
{
	if (auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx)) {
		event.Enable(frame->GetFrameIdx() != m_frame_idx);
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateDeleteFrame(wxUpdateUIEvent& event)
{
	if (auto frame = AnimHelper::GetKeyFrame(m_canim, m_layer_idx, m_frame_idx)) {
		event.Enable(frame->GetFrameIdx() != m_frame_idx);
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateInsertKeyFrame(wxUpdateUIEvent& event)
{
	if (auto layer = AnimHelper::GetLayer(m_canim, m_layer_idx)) {
		event.Enable(!layer->IsKeyFrame(m_frame_idx));
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnUpdateDeleteKeyFrame(wxUpdateUIEvent& event)
{
	if (auto layer = AnimHelper::GetLayer(m_canim, m_layer_idx)) {
		event.Enable(layer->IsKeyFrame(m_frame_idx));
	} else {
		event.Enable(false);
	}
}

void WxTimeStagePanel::OnInsertFrame(wxKeyEvent& event)
{
	OnInsertFrame();
}

void WxTimeStagePanel::OnDeleteFrame(wxKeyEvent& event)
{
	OnDeleteFrame();
}

void WxTimeStagePanel::OnInsertFrame()
{
	if (auto layer = AnimHelper::GetLayer(m_canim, m_layer_idx)) {
		layer->InsertNullFrame(m_frame_idx);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnDeleteFrame()
{
	if (auto layer = AnimHelper::GetLayer(m_canim, m_layer_idx)) {
		layer->RemoveNullFrame(m_frame_idx);
		Refresh(false);
	}
}

void WxTimeStagePanel::OnUpdateNode()
{
	int frame_idx = AnimHelper::GetCurrFrame(m_canim);
	if (frame_idx != m_frame_idx)
	{
		m_frame_idx = frame_idx;
		m_valid_frame_idx = frame_idx;
		Refresh(false);
	}
}

}
}