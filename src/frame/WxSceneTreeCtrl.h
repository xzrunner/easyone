#pragma once

#include <ee0/Observer.h>

#include <node0/typedef.h>

#include <wx/treectrl.h>

namespace ee0 { class SubjectMgr; }

namespace eone
{

class WxSceneTreePanel;
class WxSceneTreeItem;

class WxSceneTreeCtrl : public wxTreeCtrl, public ee0::Observer
{
public:
	WxSceneTreeCtrl(wxWindow* parent, ee0::SubjectMgr& sub_mgr);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	void Traverse(wxTreeItemId id, std::function<bool(wxTreeItemId)> func) const;

private:
	void InitRoot();
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void OnSelChanged(wxTreeEvent& event);
	void OnLabelEdited(wxTreeEvent& event);
	void OnBeginDrag(wxTreeEvent& event);
	void OnEndDrag(wxTreeEvent& event);

	void SelectSceneNode(const ee0::VariantSet& variants);
	void UnselectSceneNode(const ee0::VariantSet& variants);

	void InsertSceneNode(const ee0::VariantSet& variants);
	void InsertSceneNode(wxTreeItemId parent, const n0::SceneNodePtr& child,
		const n0::SceneNodePtr& root, size_t node_id);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const n0::SceneNodePtr& node);
	void ClearSceneNode();
	void ReorderSceneNode(const ee0::VariantSet& variants);
	bool ReorderItem(wxTreeItemId item, bool up);

	void StagePageChanged(const ee0::VariantSet& variants);

	void ClearALLSelected();

	void CopyChildrenTree(wxTreeItemId from, wxTreeItemId to);
	void MoveSceneNode(WxSceneTreeItem* src, const n0::SceneNodePtr& dst_parent);
	void UpdateTreeNodeID(wxTreeItemId src, wxTreeItemId dst);
	void UpdateTreeNodeID(wxTreeItemId root);

	bool GetTreePath(wxTreeItemId start, wxTreeItemId end, std::vector<wxTreeItemId>& path);
	
private:
	enum
	{
		ID_SCENE_TREE_CTRL = 1000
	};

private:
	ee0::SubjectMgr* m_sub_mgr;

	wxTreeItemId m_root;

	wxTreeItemId m_dragged_item;

	DECLARE_EVENT_TABLE()

}; // WxSceneTreeCtrl

}