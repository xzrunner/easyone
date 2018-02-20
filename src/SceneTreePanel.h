#pragma once

#include <wx/panel.h>

namespace ee0 { class SubjectMgr; }

namespace eone
{

class SceneTreePanel : public wxPanel
{
public:
	SceneTreePanel(wxWindow* parent, ee0::SubjectMgr& sub_mgr);

private:
	void InitLayout();

	void OnCreatePress(wxCommandEvent& event);

private:
	ee0::SubjectMgr& m_sub_mgr;

	wxButton* m_create_btn;

}; // SceneTreePanel

}