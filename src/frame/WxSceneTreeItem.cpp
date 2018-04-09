#include "frame/WxSceneTreeItem.h"

#include <guard/check.h>

namespace eone
{

WxSceneTreeItem::WxSceneTreeItem(const WxSceneTreeItem& item)
	: m_obj(item.m_obj)
	, m_root(item.m_root)
	, m_obj_id(item.m_obj_id)
{
}

WxSceneTreeItem::WxSceneTreeItem(const ee0::GameObj& obj, 
	                             const ee0::GameObj& root, 
	                             int obj_id)
	: m_obj(obj)
	, m_root(root)
	, m_obj_id(obj_id)
{
	GD_ASSERT((obj && root) || (!obj && !root), "err");
}

}