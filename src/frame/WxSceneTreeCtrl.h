#pragma once

#include <ee0/GameObj.h>
#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <wx/treectrl.h>

#ifdef GAME_OBJ_ECS
namespace ecsx { class World; }
#endif // GAME_OBJ_ECS

namespace eone
{

class WxSceneTreePanel;

class WxSceneTreeCtrl : public wxTreeCtrl, public ee0::Observer
{
public:
	WxSceneTreeCtrl(
		wxWindow* parent, 
		const ee0::SubjectMgrPtr& sub_mgr,
#ifdef GAME_OBJ_ECS
		ecsx::World& world,
#endif // GAME_OBJ_ECS
		const ee0::GameObj& root_obj
	);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	void Traverse(wxTreeItemId id, std::function<bool(wxTreeItemId)> func) const;

private:
	void InitRoot();
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void OnSelChanged(wxTreeEvent& event);
	void OnLabelEdited(wxTreeEvent& event);
	void OnBeginDrag(wxTreeEvent& event);
	void OnEndDrag(wxTreeEvent& event);
	void OnKeyDown(wxKeyEvent& event);

	void SelectSceneObj(const ee0::VariantSet& variants);
	void UnselectSceneObj(const ee0::VariantSet& variants);

	void SelectSceneObj(const ee0::GameObj& obj, bool multiple);
	void UnselectSceneObj(const ee0::GameObj& obj);

	void InsertSceneObj(const ee0::VariantSet& variants);
	void InsertSceneObj(wxTreeItemId parent, const ee0::GameObj& child,
		const ee0::GameObj& root, size_t obj_id);
	void DeleteSceneObj(const ee0::VariantSet& variants);
	void DeleteSceneObj(const ee0::GameObj& obj);
	void ClearSceneObj();
	void ReorderSceneObj(const ee0::VariantSet& variants);
	bool ReorderItem(wxTreeItemId item, bool up);
	void ReorderSelectionToMsg(bool up);

	void StagePageChanged(const ee0::VariantSet& variants);

	void ClearALLSelected();

	void CopyChildrenTree(wxTreeItemId from, wxTreeItemId to);
	void MoveSceneObj(wxTreeItemId src, wxTreeItemId dst_parent);
	void DeleteEmptyObjToRoot(wxTreeItemId item);

	void RebuildTree(const ee0::GameObj& obj);
	void RebuildTree(const ee0::VariantSet& variants);

	void ChangeName(const ee0::VariantSet& variants);
	
	void SelectUp();
	void SelectDown();
	void SelectLeft();
	void SelectRight();

	void DeleteSelectedObj();
	void DeleteObjOutside(wxTreeItemId item);
	void CleanRootEmptyChild();
		
	wxTreeItemId GetSingleSelected() const;

private:
	enum
	{
		ID_SCENE_TREE_CTRL = 1000
	};

private:
	ee0::SubjectMgrPtr m_sub_mgr;
#ifdef GAME_OBJ_ECS
	ecsx::World&       m_world;
#endif // GAME_OBJ_ECS
	ee0::GameObj       m_root_obj;

	wxTreeItemId m_root;

	wxTreeItemId m_dragged_item;

	bool m_disable_select;
	
	DECLARE_EVENT_TABLE()

}; // WxSceneTreeCtrl

}