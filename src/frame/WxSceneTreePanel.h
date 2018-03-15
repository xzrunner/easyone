#pragma once

#include <ee0/typedef.h>

#include <node0/typedef.h>

#include <wx/panel.h>

namespace eone
{

class WxSceneTreePanel : public wxPanel
{
public:
	WxSceneTreePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		const n0::SceneNodePtr& root_node);

private:
	void InitLayout(const n0::SceneNodePtr& root_node);

	void OnCreatePress(wxCommandEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	wxButton* m_create_btn;

}; // WxSceneTreePanel

}