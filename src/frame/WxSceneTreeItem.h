#pragma once

#include <node0/typedef.h>

#include <wx/treectrl.h>

namespace eone
{

class WxSceneTreeItem : public wxTreeItemData
{
public:
	WxSceneTreeItem(const n0::SceneNodePtr& node);

	const n0::SceneNodePtr& GetNode() const { return m_node; }

private:
	n0::SceneNodePtr m_node = nullptr;

}; // WxSceneTreeItem

}