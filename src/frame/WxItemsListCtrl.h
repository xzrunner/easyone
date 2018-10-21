#pragma once

#include <frame/WxItemsListList.h>

#include <ee0/Observer.h>
#include <ee0/typedef.h>
#include <ee0/GameObj.h>
#include <ee0/WxLibraryItem.h>

#include <guard/check.h>

#include <wx/window.h>

ECS_WORLD_DECL

class wxChoice;

namespace eone
{

class WxItemsListCtrl : public wxWindow, public ee0::Observer
{
public:
	WxItemsListCtrl(wxWindow* parent, ECS_WORLD_PARAM
		const ee0::SubjectMgrPtr& sub_mgr);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	void OnSearch(const std::string& str);

private:
	void InitLayout();

	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void OnSetFilter(wxCommandEvent& event);
	void OnSelectAll(wxCommandEvent& event);

	void InsertSceneObj(const ee0::VariantSet& variants);
	void DeleteSceneObj(const ee0::VariantSet& variants);
	void ClearSceneObj();
	void ReorderSceneObj(const ee0::VariantSet& variants);

	void SelectSceneObj(const ee0::VariantSet& variants);
	void UnselectSceneObj(const ee0::VariantSet& variants);
	void ClearALLSelected();

	void StagePageChanged(const ee0::VariantSet& variants);

	int QueryItemIndex(const ee0::GameObj& obj) const;

private:
	ECS_WORLD_SELF_DEF
	ee0::SubjectMgrPtr m_sub_mgr;

	wxChoice* m_filter;

	WxItemsListList* m_list;

}; // WxItemsListCtrl

}