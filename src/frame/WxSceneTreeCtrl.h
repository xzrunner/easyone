#pragma once

#include <ee0/GameObj.h>
#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <guard/check.h>

#include <wx/treectrl.h>

ECS_WORLD_DECL

namespace eone
{

class WxSceneTreeCtrl : public wxTreeCtrl, public ee0::Observer
{
public:
	WxSceneTreeCtrl(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		ECS_WORLD_PARAM const ee0::GameObj& root_obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	void Traverse(wxTreeItemId id, std::function<bool(wxTreeItemId)> func) const;

	void OnSearch(const std::string& str);

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
	class Item : public wxTreeItemData
	{
	public:
		Item()
			: m_obj_id(-1)
#ifndef GAME_OBJ_ECS
			, m_obj(nullptr)
			, m_root(nullptr)
#endif // GAME_OBJ_ECS
		{}
		Item(const Item& item)
			: m_obj(item.m_obj)
			, m_root(item.m_root)
			, m_obj_id(item.m_obj_id)
		{}
		Item(const ee0::GameObj& obj, const ee0::GameObj& root, int obj_id)
			: m_obj(obj)
			, m_root(root)
			, m_obj_id(obj_id)
		{
			GD_ASSERT((GAME_OBJ_VALID(obj) && GAME_OBJ_VALID(root)) || (!GAME_OBJ_VALID(obj) && !GAME_OBJ_VALID(root)), "err");
		}

		const ee0::GameObj& GetObj() const { return m_obj; }

		const ee0::GameObj& GetRoot() const { return m_root; }

		int GetObjID() const { return m_obj_id; }
		void SetObjID(int id) { m_obj_id = id; }

	private:
		ee0::GameObj m_obj;

		ee0::GameObj m_root;
		int m_obj_id;

	}; // Item

private:
	ee0::SubjectMgrPtr m_sub_mgr;
	ECS_WORLD_SELF_DEF
	ee0::GameObj       m_root_obj;

	wxTreeItemId m_root;

	wxTreeItemId m_dragged_item;

	bool m_disable_select;
    bool m_disable_on_sel_changed = false;

	DECLARE_EVENT_TABLE()

}; // WxSceneTreeCtrl

}