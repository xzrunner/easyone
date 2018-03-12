#pragma once

#include <node0/typedef.h>

#include <wx/treectrl.h>

namespace eone
{

class WxSceneTreeItem : public wxTreeItemData
{
public:
	WxSceneTreeItem(const WxSceneTreeItem& item);
	WxSceneTreeItem(const n0::SceneNodePtr& node, 
		const n0::SceneNodePtr& root, int node_id);

	const n0::SceneNodePtr& GetNode() const { return m_node; }

	const n0::SceneNodePtr& GetRoot() const { return m_root; }
	int GetNodeID() const { return m_node_id; }

private:
	n0::SceneNodePtr m_node = nullptr;

	n0::SceneNodePtr m_root = nullptr;
	int m_node_id = -1;

}; // WxSceneTreeItem

}