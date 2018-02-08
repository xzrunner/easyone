#pragma once

#include <node0/typedef.h>

#include <wx/treectrl.h>

namespace eone
{

class SceneTreeItem : public wxTreeItemData
{
public:
	SceneTreeItem();
	SceneTreeItem(const n0::SceneNodePtr& node);

	void AddChild(SceneTreeItem* item);

	size_t GetChildrenNum() const { return m_children.size(); }

	const n0::SceneNodePtr& GetNode() const { return m_node; }

private:
	n0::SceneNodePtr m_node = nullptr;

	std::vector<SceneTreeItem*> m_children;

}; // SceneTreeItem

}