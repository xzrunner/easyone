#include "SceneTreeItem.h"

namespace eone
{

SceneTreeItem::SceneTreeItem()
{
}

SceneTreeItem::SceneTreeItem(const n0::SceneNodePtr& node)
	: m_node(node)
{
}

void SceneTreeItem::AddChild(SceneTreeItem* item)
{
	m_children.push_back(item);
}

}