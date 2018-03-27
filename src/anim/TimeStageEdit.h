#pragma once

#include <ee0/typedef.h>

namespace n2 { class CompAnim; }

namespace eone
{
namespace anim
{

class TimeStageEdit
{
public:
	TimeStageEdit(const n2::CompAnim& canim,
		const ee0::SubjectMgrPtr& tl_sub_mgr);

	void OnMouseLeftDown(int row, int col);
	void OnMouseDragging(int row, int col);
	void OnMouseLeftClick(int row, int col);

	void CopySelection();
	void PasteSelection();
	void DeleteSelection();

private:
	void UpdateRegion(int layer, int frame);

private:
	const n2::CompAnim& m_canim;
	ee0::SubjectMgrPtr  m_tl_sub_mgr;

	int m_row, m_col;
	int m_col_min, m_col_max;

}; // TimeStageEdit

}
}