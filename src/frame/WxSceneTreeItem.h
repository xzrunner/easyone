#pragma once

#include <ee0/GameObj.h>

#include <wx/treectrl.h>

namespace eone
{

class WxSceneTreeItem : public wxTreeItemData
{
public:
	WxSceneTreeItem(const WxSceneTreeItem& item);
	WxSceneTreeItem(const ee0::GameObj& obj, 
		const ee0::GameObj& root, int obj_id);

	const ee0::GameObj& GetObj() const { return m_obj; }

	const ee0::GameObj& GetRoot() const { return m_root; }

	int GetObjID() const { return m_obj_id; }
	void SetObjID(int id) { m_obj_id = id; }

private:
	ee0::GameObj m_obj = nullptr;

	ee0::GameObj m_root = nullptr;
	int m_obj_id = -1;

}; // WxSceneTreeItem

}