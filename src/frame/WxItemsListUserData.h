#pragma once

#include <ee0/WxLibraryItem.h>
#include <ee0/GameObj.h>

#include <guard/check.h>

namespace eone
{

class WxItemsListUserData : public ee0::WxLibraryItem::UserData
{
public:
	WxItemsListUserData()
		: m_obj_id(-1)
#ifndef GAME_OBJ_ECS
		, m_obj(nullptr)
		, m_root(nullptr)
#endif // GAME_OBJ_ECS
	{}
	WxItemsListUserData(const WxItemsListUserData& item)
		: m_obj(item.m_obj)
		, m_root(item.m_root)
		, m_obj_id(item.m_obj_id)
	{}
	WxItemsListUserData(const ee0::GameObj& obj, const ee0::GameObj& root, int obj_id)
		: m_obj(obj)
		, m_root(root)
		, m_obj_id(obj_id)
	{
		GD_ASSERT((GAME_OBJ_VALID(obj) && GAME_OBJ_VALID(root)) || (!GAME_OBJ_VALID(obj) && !GAME_OBJ_VALID(root)), "err");
	}

	const ee0::GameObj& GetObj() const { return m_obj; }

	const ee0::GameObj& GetRoot() const { return m_root; }

	int GetObjID() const { return m_obj_id; }
	void SetObjID(int id) { m_obj_id = id; }

private:
	ee0::GameObj m_obj;

	ee0::GameObj m_root;
	int m_obj_id;

}; // WxItemsListUserData	

}