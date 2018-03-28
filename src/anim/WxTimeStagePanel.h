#pragma once

#include "anim/TimeStageEdit.h"
#include "anim/config.h"

#include <ee0/Observer.h>

#include <wx/panel.h>

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
	virtual ~WxTimeStagePanel();

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

	bool InsertSceneNode(const ee0::VariantSet& variants);
	bool DeleteSceneNode(const ee0::VariantSet& variants);
	bool ClearSceneNode();
	bool ReorderSceneNode(const ee0::VariantSet& variants);

	void OnSetCurrFrame(const ee0::VariantSet& variants);
	void OnSetSelectedRegion(const ee0::VariantSet& variants);

	void MousePopupMenu(int x, int y);

	void OnCreateClassicTween(wxCommandEvent& event);
	void OnDeleteClassicTween(wxCommandEvent& event);
	void OnInsertFrame(wxCommandEvent& event);
	void OnDeleteFrame(wxCommandEvent& event);
	void OnInsertKeyFrame(wxCommandEvent& event);
	void OnDeleteKeyFrame(wxCommandEvent& event);

	void OnUpdateCreateClassicTween(wxUpdateUIEvent& event);
	void OnUpdateDeleteClassicTween(wxUpdateUIEvent& event);
	void OnUpdateInsertFrame(wxUpdateUIEvent& event);
	void OnUpdateDeleteFrame(wxUpdateUIEvent& event);
	void OnUpdateInsertKeyFrame(wxUpdateUIEvent& event);
	void OnUpdateDeleteKeyFrame(wxUpdateUIEvent& event);

	void OnInsertFrame(wxKeyEvent& event);
	void OnDeleteFrame(wxKeyEvent& event);

private:
	void OnInsertFrame();
	void OnDeleteFrame();

	void OnUpdateNode();

private:
	enum
	{
		Menu_CreateClassicTween,
		Menu_DeleteClassicTween,
		Menu_InsertFrame,
		Menu_DeleteFrame,
		Menu_InsertKeyFrame,
		Menu_DeleteKeyFrame,

		Hot_InsertFrame,
		Hot_DeleteFrame
	};

	static LanguageEntry entries[];

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