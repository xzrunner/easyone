#pragma once

#include <ee0/Observer.h>
#include <ee0/typedef.h>

#include <node0/typedef.h>

#include <wx/treectrl.h>

namespace eone
{

class WxSceneTreePanel;

class WxSceneTreeCtrl : public wxTreeCtrl, public ee0::Observer
{
public:
	WxSceneTreeCtrl(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
		const n0::SceneNodePtr& root_node);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	void Traverse(wxTreeItemId id, std::function<bool(wxTreeItemId)> func) const;

private:
	void InitRoot();
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void OnSelChanged(wxTreeEvent& event);
	void OnLabelEdited(wxTreeEvent& event);
	void OnBeginDrag(wxTreeEvent& event);
	void OnEndDrag(wxTreeEvent& event);
	void OnKeyDown(wxKeyEvent& event);

	void SelectSceneNode(const ee0::VariantSet& variants);
	void UnselectSceneNode(const ee0::VariantSet& variants);

	void SelectSceneNode(const n0::SceneNodePtr& node, bool multiple);
	void UnselectSceneNode(const n0::SceneNodePtr& node);

	void InsertSceneNode(const ee0::VariantSet& variants);
	void InsertSceneNode(wxTreeItemId parent, const n0::SceneNodePtr& child,
		const n0::SceneNodePtr& root, size_t node_id);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const n0::SceneNodePtr& node);
	void ClearSceneNode();
	void ReorderSceneNode(const ee0::VariantSet& variants);
	bool ReorderItem(wxTreeItemId item, bool up);
	void ReorderSelectionToMsg(bool up);

	void StagePageChanged(const ee0::VariantSet& variants);

	void ClearALLSelected();

	void CopyChildrenTree(wxTreeItemId from, wxTreeItemId to);
	void MoveSceneNode(wxTreeItemId src, wxTreeItemId dst_parent);
	void DeleteEmptyNodeToRoot(wxTreeItemId item);

	void RebuildTree(const n0::SceneNodePtr& node);

	void ChangeName(const ee0::VariantSet& variants);
	
	void SelectUp();
	void SelectDown();
	void SelectLeft();
	void SelectRight();

	void DeleteSelectedNode();
	void DeleteNodeOutside(wxTreeItemId item);
	void CleanRootEmptyChild();
		
	wxTreeItemId GetSingleSelected() const;

private:
	enum
	{
		ID_SCENE_TREE_CTRL = 1000
	};

private:
	ee0::SubjectMgrPtr m_sub_mgr;
	n0::SceneNodePtr   m_root_node;

	wxTreeItemId m_root;

	wxTreeItemId m_dragged_item;

	bool m_disable_select;
	
	DECLARE_EVENT_TABLE()

}; // WxSceneTreeCtrl

}