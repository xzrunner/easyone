#include "anim/TimeStageEdit.h"
#include "anim/MessageID.h"
#include "anim/MessageHelper.h"
#include "anim/AnimHelper.h"

#include <ee0/SubjectMgr.h>

#include <node2/CompAnim.h>

#include <wx/utils.h>

namespace eone
{
namespace anim
{

TimeStageEdit::TimeStageEdit(const n2::CompAnim& canim,
	                         const ee0::SubjectMgrPtr& sub_mgr)
	: m_canim(canim)
	, m_sub_mgr(sub_mgr)
{
	m_row = m_col = -1;
	m_col_min = m_col_max = -1;
}

void TimeStageEdit::OnMouseLeftDown(int row, int col)
{
	int layer_sz = m_canim.GetAllLayers().size();
	if (row < 0 || row >= layer_sz) {
		return;
	}

	int layer_idx = layer_sz - row - 1;
	if (wxGetKeyState(WXK_SHIFT))
	{
		UpdateRegion(layer_idx, col);
		MessageHelper::SetSelectedRegion(*m_sub_mgr, m_col);
	} 
	else 
	{
		m_row = row;
		m_col = col;
		m_col_min = m_col_max = col;
		MessageHelper::SetCurrFrame(*m_sub_mgr, layer_idx, col);
	}
}

void TimeStageEdit::OnMouseDragging(int row, int col)
{

}

void TimeStageEdit::OnMouseLeftClick(int row, int col)
{

}

void TimeStageEdit::CopySelection()
{

}

void TimeStageEdit::PasteSelection()
{

}

void TimeStageEdit::DeleteSelection()
{

}

void TimeStageEdit::UpdateRegion(int layer_idx, int frame_idx)
{
	int max_frame = AnimHelper::GetMaxFrame(m_canim, layer_idx);
	if (max_frame >= 0) {
		m_col_min = std::min(std::min(m_col, frame_idx), max_frame);
		m_col_max = std::min(std::max(m_col, frame_idx), max_frame);
	}
}

}
}