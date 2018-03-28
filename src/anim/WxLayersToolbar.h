#pragma once

#include <ee0/typedef.h>

#include <wx/panel.h>

namespace n2 { class CompAnim; }

namespace eone
{
namespace anim
{

class WxLayersToolbar : public wxPanel
{
public:
	WxLayersToolbar(wxWindow* parent, n2::CompAnim& canim,
		const ee0::SubjectMgrPtr& sub_mgr);

private:
	void InitLayout();

	void OnAddLayer(wxCommandEvent& event);
	void OnDelLayer(wxCommandEvent& event);

	void OnPressPlay(wxCommandEvent& event);

private:
	n2::CompAnim&      m_canim;
	ee0::SubjectMgrPtr m_sub_mgr;

	wxButton* m_btn_play;

}; // WxLayersToolbar

}
}