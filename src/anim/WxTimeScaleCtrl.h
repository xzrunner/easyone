#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

namespace n2 { class CompAnim; class CompAnimInst; }

namespace eone
{
namespace anim
{

class WxTimeScaleCtrl : public wxPanel, public ee0::Observer
{
public:
	WxTimeScaleCtrl(wxWindow* parent, const n2::CompAnim& canim,
		const n2::CompAnimInst& canim_inst, const ee0::SubjectMgrPtr& sub_mgr);
	virtual ~WxTimeScaleCtrl();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

private:
	void OnSize(wxSizeEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnMouse(wxMouseEvent& event);

	void OnUpdateNode();
	void OnSetCurrFrame(const ee0::VariantSet& variants);
	void OnWndScroll(const ee0::VariantSet& variants);

private:
	const n2::CompAnim&     m_canim;
	const n2::CompAnimInst& m_canim_inst;
	ee0::SubjectMgrPtr      m_sub_mgr;

	int m_frame_idx;

	int m_start_x;

	DECLARE_EVENT_TABLE()

}; // WxTimeScaleCtrl

}
}