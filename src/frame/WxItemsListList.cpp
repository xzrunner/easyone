#include "frame/WxItemsListList.h"
#include "frame/WxItemsListUserData.h"

#include <ee0/MsgHelper.h>
#include <ee0/MessageID.h>
#include <ee0/SubjectMgr.h>

namespace eone
{

WxItemsListList::WxItemsListList(wxWindow* parent,
	                             const ee0::SubjectMgrPtr& sub_mgr)
	: ee0::WxImageVList(parent, "", true, false, true)
	, m_sub_mgr(sub_mgr)
{
}

void WxItemsListList::OnListSelected(wxCommandEvent& event)
{
	if (!GetKeyState(WXK_CONTROL)) {
		m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	}
	int idx = event.GetInt();

	std::vector<ee0::GameObjWithPos> objs;
	auto item = GetItemByIndex(idx);
	if (!item) {
		return;
	}

	auto& obj = *dynamic_cast<WxItemsListUserData*>(item->GetUD().get());
#ifndef GAME_OBJ_ECS
	objs.push_back(n0::NodeWithPos(obj.GetObj(), obj.GetRoot(), obj.GetObjID()));
#else
	objs.push_back(obj.GetObj());
#endif // GAME_OBJ_ECS

	ee0::MsgHelper::InsertSelection(*m_sub_mgr, objs);
}

}