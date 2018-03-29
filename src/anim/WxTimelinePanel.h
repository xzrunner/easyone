#pragma once

#include <ee0/typedef.h>

#include <wx/panel.h>

namespace n2 { class CompAnim; class CompAnimInst; }

namespace eone
{
namespace anim
{

class WxTimelinePanel : public wxPanel
{
public:
	WxTimelinePanel(wxWindow* parent, n2::CompAnim& canim,
		n2::CompAnimInst& canim_inst, const ee0::SubjectMgrPtr& sub_mgr);

private:
	void InitLayout(n2::CompAnim& canim, n2::CompAnimInst& canim_inst,
		const ee0::SubjectMgrPtr& sub_mgr);

}; // WxTimelinePanel

}
}