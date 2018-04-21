#pragma once

#include <ee0/GameObj.h>
#include <ee0/typedef.h>

#include <wx/panel.h>

class wxNotebook;
class wxSearchCtrl;

namespace eone
{

class WxSceneTreeCtrl;
class WxItemsListCtrl;

class WxWorldPanel : public wxPanel
{
public:
	WxWorldPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		ECS_WORLD_PARAM const ee0::GameObj& root_obj);

private:
	void InitLayout(const ee0::SubjectMgrPtr& sub_mgr,
		ECS_WORLD_PARAM const ee0::GameObj& root_obj);

	void OnAddPress(wxCommandEvent& event);
	void OnSearch(wxCommandEvent& event);

private:
	ee0::SubjectMgrPtr m_sub_mgr;

	wxButton*     m_create_btn;
	wxSearchCtrl* m_search_ctrl;

	wxNotebook* m_notebook;

	WxSceneTreeCtrl* m_tree_page;
	WxItemsListCtrl* m_list_page;

}; // WxWorldPanel

}