#include "frame/WxSceneTreeItem.h"

#include <guard/check.h>

namespace eone
{

WxSceneTreeItem::WxSceneTreeItem()
	: m_obj_id(-1)
#ifndef GAME_OBJ_ECS
	, m_obj(nullptr)
	, m_root(nullptr)
#endif // GAME_OBJ_ECS
{
}

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
	GD_ASSERT((GAME_OBJ_VALID(obj) && GAME_OBJ_VALID(root)) || (!GAME_OBJ_VALID(obj) && !GAME_OBJ_VALID(root)), "err");
}

}