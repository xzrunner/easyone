#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/scrolwin.h>

namespace n2 { class CompAnim; class CompAnimInst; }

namespace eone
{
namespace anim
{

class WxTimeStageScrolled : public wxScrolledWindow
{
public:
	WxTimeStageScrolled(wxWindow* parent, const n2::CompAnim& canim,
		const n2::CompAnimInst& canim_inst, const ee0::SubjectMgrPtr& sub_mgr);

private:
	void InitLayout(const n2::CompAnim& canim, const n2::CompAnimInst& canim_inst,
		const ee0::SubjectMgrPtr& sub_mgr);

	void OnScroll(wxScrollWinEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	DECLARE_EVENT_TABLE()

}; // WxTimeStageScrolled

}
}