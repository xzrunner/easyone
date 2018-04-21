#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>
#include <ee0/GameObj.h>

#include <wx/window.h>

namespace ee0 { class WxImageVList; }
ECS_WORLD_DECL

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

	void InsertSceneObj(const ee0::VariantSet& variants);
	void DeleteSceneObj(const ee0::VariantSet& variants);
	void ClearSceneObj();
	void ReorderSceneObj(const ee0::VariantSet& variants);

	void SelectSceneObj(const ee0::VariantSet& variants);
	void UnselectSceneObj(const ee0::VariantSet& variants);
	void ClearALLSelected();

	void StagePageChanged(const ee0::VariantSet& variants);

	static std::string GetItemFilepath(const ee0::GameObj& obj);

private:
	ECS_WORLD_SELF_DEF
	ee0::SubjectMgrPtr m_sub_mgr;

	ee0::WxImageVList* m_list;

}; // WxItemsListCtrl

}