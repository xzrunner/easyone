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
	WxTimelinePanel(wxWindow* parent, const n2::CompAnim& canim);

private:
	void InitLayout(const n2::CompAnim& canim);

private:
	ee0::SubjectMgrPtr m_tl_sub_mgr = nullptr;

}; // WxTimelinePanel

}
}