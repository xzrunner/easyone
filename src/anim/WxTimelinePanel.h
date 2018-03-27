#pragma once

#include <ee0/typedef.h>

#include <wx/panel.h>

namespace n2 { class CompAnim; }

namespace eone
{
namespace anim
{

class WxTimelinePanel : public wxPanel
{
public:
	WxTimelinePanel(wxWindow* parent, n2::CompAnim& canim,
		const ee0::SubjectMgrPtr& sub_mgr);

private:
	void InitLayout(n2::CompAnim& canim,
		const ee0::SubjectMgrPtr& sub_mgr);

private:
	ee0::SubjectMgrPtr m_tl_sub_mgr = nullptr;

}; // WxTimelinePanel

}
}