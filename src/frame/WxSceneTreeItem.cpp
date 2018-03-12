#include "frame/WxSceneTreeItem.h"

namespace eone
{

WxSceneTreeItem::WxSceneTreeItem(const WxSceneTreeItem& item)
	: m_node(item.m_node)
	, m_root(item.m_root)
	, m_node_id(item.m_node_id)
{
}

WxSceneTreeItem::WxSceneTreeItem(const n0::SceneNodePtr& node, 
	                             const n0::SceneNodePtr& root, 
	                             int node_id)
	: m_node(node)
	, m_root(root)
	, m_node_id(node_id)
{
}

}