#pragma once

#include "anim/TimeStageEdit.h"

#include <ee0/Observer.h>

#include <wx/panel.h>

namespace n2 { class CompAnim; }
namespace ee0 { class VariantSet; }

class wxBufferedPaintDC;

namespace eone
{
namespace anim
{

class WxTimeStagePanel : public wxPanel, public ee0::Observer
{
public:
	WxTimeStagePanel(wxWindow* parent, const n2::CompAnim& canim,
		const ee0::SubjectMgrPtr& sub_mgr);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnMouse(wxMouseEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);

	void DrawBackground(wxBufferedPaintDC& dc);
	void DrawLayersDataBg(wxBufferedPaintDC& dc);
	void DrawLayersDataFlag(wxBufferedPaintDC& dc);
	void DrawCurrPosFlag(wxBufferedPaintDC& dc);
	void DrawSelected(wxBufferedPaintDC& dc);

	void OnSetCurrFrame(const ee0::VariantSet& variants);
	void OnSetSelectedRegion(const ee0::VariantSet& variants);

private:
	const n2::CompAnim& m_canim;
	ee0::SubjectMgrPtr  m_sub_mgr;

	TimeStageEdit m_editop;

	int m_layer_idx, m_frame_idx;
	int m_valid_frame_idx;
	int m_col_min, m_col_max;

	DECLARE_EVENT_TABLE()

}; // WxTimeStagePanel

}
}