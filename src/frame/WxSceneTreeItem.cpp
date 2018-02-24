#include "frame/WxSceneTreeItem.h"

namespace eone
{

WxSceneTreeItem::WxSceneTreeItem()
{
}

WxSceneTreeItem::WxSceneTreeItem(const n0::SceneNodePtr& node)
	: m_node(node)
{
}

void WxSceneTreeItem::AddChild(WxSceneTreeItem* item)
{
	m_children.push_back(item);
}

}