#pragma once

#include <ee0/GameObj.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

ECS_WORLD_DECL

namespace eone
{

class WxSceneTreePanel : public wxPanel
{
public:
	WxSceneTreePanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		ECS_WORLD_PARAM const ee0::GameObj& root_obj);

private:
	void InitLayout(const ee0::GameObj& root_obj);

	void OnCreatePress(wxCommandEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;
	ECS_WORLD_SELF_DEF

	wxButton* m_create_btn;

}; // WxSceneTreePanel

}