#include "frame/WxSceneTreeCtrl.h"
#include "frame/WxStagePage.h"
#include "frame/MessageID.h"
#include "frame/config.h"

#include <ee0/SubjectMgr.h>
#ifndef GAME_OBJ_ECS
#else
#include <ee0/CompEntityEditor.h>
#endif // GAME_OBJ_ECS
#include <ee0/MsgHelper.h>
#include <ee2/NodeReorderHelper.h>

#include <guard/check.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node0/CompAsset.h>
#include <node0/CompIdentity.h>
#include <node0/SceneTreeHelper.h>
#include <node0/CompComplex.h>
#include <node2/CompTransform.h>
#include <node2/SceneTreeHelper.h>
#else
#include <entity0/World.h>
#include <entity2/CompComplex.h>
#endif // GAME_OBJ_ECS

#include <queue>

namespace eone
{


BEGIN_EVENT_TABLE(WxSceneTreeCtrl, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(ID_SCENE_TREE_CTRL, WxSceneTreeCtrl::OnSelChanged)
	EVT_TREE_END_LABEL_EDIT(ID_SCENE_TREE_CTRL, WxSceneTreeCtrl::OnLabelEdited)
	EVT_TREE_BEGIN_DRAG(ID_SCENE_TREE_CTRL, WxSceneTreeCtrl::OnBeginDrag)
	EVT_TREE_END_DRAG(ID_SCENE_TREE_CTRL, WxSceneTreeCtrl::OnEndDrag)
	EVT_KEY_DOWN(WxSceneTreeCtrl::OnKeyDown)
END_EVENT_TABLE()

WxSceneTreeCtrl::WxSceneTreeCtrl(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
	                             ECS_WORLD_PARAM const ee0::GameObj& root_obj)
	: wxTreeCtrl(parent, ID_SCENE_TREE_CTRL, wxDefaultPosition, wxDefaultSize,
		/*wxTR_HIDE_ROOT | */wxTR_EDIT_LABELS | wxTR_MULTIPLE | wxTR_NO_LINES | wxTR_DEFAULT_STYLE)
	, m_sub_mgr(sub_mgr)
	ECS_WORLD_SELF_ASSIGN
	, m_root_obj(root_obj)
	, m_disable_select(false)
{
	SetBackgroundColour(PANEL_COLOR);

	InitRoot();
	RegisterMsg(*m_sub_mgr);
}

void WxSceneTreeCtrl::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_SCENE_NODE_INSERT:
		InsertSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
		DeleteSceneObj(variants);
		break;
	case ee0::MSG_SCENE_NODE_CLEAR:
		ClearSceneObj();
		break;
	case ee0::MSG_SCENE_NODE_UP_DOWN:
		ReorderSceneObj(variants);
		break;

	case ee0::MSG_NODE_SELECTION_INSERT:
		if (!m_disable_select) {
            m_disable_on_sel_changed = true;
			SelectSceneObj(variants);
            m_disable_on_sel_changed = false;
		}
		break;
	case ee0::MSG_NODE_SELECTION_DELETE:
		UnselectSceneObj(variants);
		break;
	case ee0::MSG_NODE_SELECTION_CLEAR:
		ClearALLSelected();
		break;
	case ee0::MSG_STAGE_PAGE_CHANGED:
		StagePageChanged(variants);
		break;

	case ee0::MSG_UPDATE_NODE_NAME:
		ChangeName(variants);
		break;

	case MSG_TREE_PANEL_REBUILD:
		RebuildTree(variants);
		break;
	}
}

void WxSceneTreeCtrl::OnSearch(const std::string& str)
{
	ClearALLSelected();
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		std::string name = GetItemText(item);
		if (name.find(str) != std::string::npos)
		{
			m_disable_select = true;
			SelectItem(item, false);
			m_disable_select = false;
		}
		return true;
	});
}

void WxSceneTreeCtrl::Traverse(wxTreeItemId id, std::function<bool(wxTreeItemId)> func) const
{
	std::queue<wxTreeItemId> buf;
	buf.push(id);
	while (!buf.empty())
	{
		wxTreeItemId item = buf.front(); buf.pop();

		wxTreeItemIdValue cookie;
		wxTreeItemId id = GetFirstChild(item, cookie);
		if (id.IsOk())
		{
			if (item != id)
			{
				// visit index
				if (!func(item)) {
					break;
				}
			}

			while (id.IsOk()) {
				buf.push(id);
				id = GetNextSibling(id);
			}
		}
		else
		{
			// visit leaf
			if (!func(item)) {
				break;
			}
		}
	}
}

void WxSceneTreeCtrl::InitRoot()
{
	m_root = AddRoot("ROOT");
	SetItemData(m_root, new Item());
	SetItemBold(m_root, true);
}

void WxSceneTreeCtrl::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_SCENE_NODE_INSERT, this);
	sub_mgr.RegisterObserver(ee0::MSG_SCENE_NODE_DELETE, this);
	sub_mgr.RegisterObserver(ee0::MSG_SCENE_NODE_CLEAR, this);
	sub_mgr.RegisterObserver(ee0::MSG_SCENE_NODE_UP_DOWN, this);

	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_DELETE, this);
	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
	sub_mgr.RegisterObserver(ee0::MSG_STAGE_PAGE_CHANGED, this);

	sub_mgr.RegisterObserver(ee0::MSG_UPDATE_NODE_NAME, this);

	sub_mgr.RegisterObserver(MSG_TREE_PANEL_REBUILD, this);
}

void WxSceneTreeCtrl::OnSelChanged(wxTreeEvent& event)
{
    if (m_disable_on_sel_changed) {
        return;
    }

	auto id = event.GetItem();
	if (!id.IsOk() || id == m_root) {
		return;
	}

	auto data = (Item*)GetItemData(id);
	auto& obj = data->GetObj();
	GD_ASSERT(GAME_OBJ_VALID(obj), "err model obj.");

	ee0::VariantSet vars;

	ee0::Variant var_obj;
	var_obj.m_type = ee0::VT_PVOID;
#ifndef GAME_OBJ_ECS
	var_obj.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(obj);
#else
	var_obj.m_val.pv = &const_cast<e0::Entity&>(obj);
#endif // GAME_OBJ_ECS
	vars.SetVariant("obj", var_obj);

	ee0::Variant var_root;
	var_root.m_type = ee0::VT_PVOID;
#ifndef GAME_OBJ_ECS
	var_root.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(data->GetRoot());
#else
	var_root.m_val.pv = &const_cast<e0::Entity&>(data->GetRoot());
#endif // GAME_OBJ_ECS
	vars.SetVariant("root", var_root);

	ee0::Variant var_id;
	var_id.m_type = ee0::VT_ULONG;
	var_id.m_val.ul = data->GetObjID();
	vars.SetVariant("id", var_id);

	ee0::Variant var_skip;
	var_skip.m_type = ee0::VT_PVOID;
	var_skip.m_val.pv = static_cast<Observer*>(this);
	vars.SetVariant("skip_observer", var_skip);

	if (IsSelected(id))
	{
		bool add = wxGetKeyState(WXK_CONTROL);
		if (!add)
		{
			ee0::Variant var_clear;
			var_clear.m_type = ee0::VT_BOOL;
			var_clear.m_val.bl = true;
			vars.SetVariant("clear", var_clear);
		}

		m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_INSERT, vars);
	}
	else
	{
		m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_DELETE, vars);
	}
}

void WxSceneTreeCtrl::OnLabelEdited(wxTreeEvent& event)
{
	wxTreeItemId edited_item = event.GetItem();
	auto data_dst = (Item*)GetItemData(edited_item);
	if (!data_dst) {
		return;
	}

#ifndef GAME_OBJ_ECS
	auto& cid = data_dst->GetObj()->GetUniqueComp<n0::CompIdentity>();
	cid.SetName(event.GetLabel().ToStdString());
#else
	auto& ceditor = m_world.GetComponent<ee0::CompEntityEditor>(data_dst->GetObj());
	ceditor.name = std::make_unique<std::string>(event.GetLabel().ToStdString());
#endif // GAME_OBJ_ECS
}

void WxSceneTreeCtrl::OnBeginDrag(wxTreeEvent& event)
{
	m_dragged_item = event.GetItem();
	event.Allow();
}

void WxSceneTreeCtrl::OnEndDrag(wxTreeEvent& event)
{
	wxTreeItemId old_item = m_dragged_item,
		         new_item_parent = event.GetItem();
	if (!old_item.IsOk() || !new_item_parent.IsOk()) {
		return;
	}
	auto data_dst = (Item*)GetItemData(new_item_parent);
	if (!data_dst) {
		return;
	}

	wxTreeItemId new_item;

	// old info
	auto data_src = (Item*)GetItemData(old_item);
	std::string name = GetItemText(old_item);
	auto old_obj = data_src->GetObj();

	// move to root
	if (!GAME_OBJ_VALID(data_dst->GetObj()))
	{
		new_item = InsertItem(new_item_parent, 0, name.c_str(), -1, -1,
			new Item(data_src->GetObj(), data_src->GetObj(), 0));
	}
	else
	{
#ifndef GAME_OBJ_ECS
		if (!data_dst->GetObj()->HasSharedComp<n0::CompComplex>()) {
#else
		if (!m_world.HasComponent<e2::CompComplex>(data_dst->GetObj())) {
#endif // GAME_OBJ_ECS
			return;
		}

#ifndef GAME_OBJ_ECS
		auto& dst_casset = data_dst->GetObj()->GetSharedComp<n0::CompAsset>();
		int new_obj_id = data_dst->GetObjID() + dst_casset.GetNodeCount();
#else
		int new_obj_id = -1;
#endif // GAME_OBJ_ECS
		new_item = InsertItem(new_item_parent, 0, name.c_str(), -1, -1,
			new Item(data_src->GetObj(), data_dst->GetRoot(), new_obj_id));
	}

#ifndef GAME_OBJ_ECS
	SetItemBold(new_item, data_src->GetObj()->HasSharedComp<n0::CompComplex>());
#else
	SetItemBold(new_item, m_world.HasComponent<e2::CompComplex>(data_src->GetObj()));
#endif // GAME_OBJ_ECS
	// move model tree
	MoveSceneObj(old_item, new_item_parent);
	// rebuild tree
	RebuildTree(m_root_obj);
	// select
	SelectSceneObj(old_obj, false);
}

void WxSceneTreeCtrl::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
	case WXK_UP:
		SelectUp();
		break;
	case WXK_DOWN:
		SelectDown();
		break;
	case WXK_LEFT:
		SelectLeft();
		break;
	case WXK_RIGHT:
		SelectRight();
		break;
	case WXK_PAGEUP:
		ReorderSelectionToMsg(true);
		break;
	case WXK_PAGEDOWN:
		ReorderSelectionToMsg(false);
		break;
	case WXK_DELETE:
		DeleteSelectedObj();
		break;
	}
}

void WxSceneTreeCtrl::SelectSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err model obj");

	bool multiple = false;
	auto var_multi = variants.GetVariant("multiple");
	if (var_multi.m_type == ee0::VT_BOOL && var_multi.m_val.bl) {
		multiple = true;
	}

	SelectSceneObj(*obj, multiple);
}

void WxSceneTreeCtrl::UnselectSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err model obj");

	UnselectSceneObj(*obj);
}

void WxSceneTreeCtrl::SelectSceneObj(const ee0::GameObj& obj, bool multiple)
{
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (Item*)GetItemData(item);
		if (pdata->GetObj() == obj)
		{
			m_disable_select = true;
			SelectItem(item, !multiple);
			m_disable_select = false;
			return false;
		}
		else
		{
			return true;
		}
	});
}

void WxSceneTreeCtrl::UnselectSceneObj(const ee0::GameObj& obj)
{
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (Item*)GetItemData(item);
		if (pdata->GetObj() == obj)
		{
			m_disable_select = true;
			UnselectItem(item);
			m_disable_select = false;
			return false;
		}
		else
		{
			return true;
		}
	});
}

// insert to root
void WxSceneTreeCtrl::InsertSceneObj(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err model obj");

	InsertSceneObj(m_root, *obj, *obj, 0);

	Expand(m_root);
}

void WxSceneTreeCtrl::InsertSceneObj(wxTreeItemId parent, const ee0::GameObj& obj,
	                                  const ee0::GameObj& root, size_t obj_id)
{
	auto item = new Item(obj, root, obj_id);
#ifndef GAME_OBJ_ECS
	auto& name = obj->GetUniqueComp<n0::CompIdentity>().GetName();
	bool is_complex = obj->HasSharedComp<n0::CompComplex>();
#else
	std::string name;
	auto& ceditor = m_world.GetComponent<ee0::CompEntityEditor>(obj);
	if (ceditor.name) {
		name = *ceditor.name;
	}
	bool is_complex = m_world.HasComponent<e2::CompComplex>(obj);
#endif // GAME_OBJ_ECS
	wxTreeItemId id = InsertItem(parent, 0, name);
	SetItemBold(id, is_complex);
	SetItemData(id, item);
	if (is_complex)
	{
#ifndef GAME_OBJ_ECS
		auto& ccomplex = obj->GetSharedComp<n0::CompComplex>();
		auto& children = ccomplex.GetAllChildren();
		obj_id = obj_id + 1;
#else
		auto& ccomplex = m_world.GetComponent<e2::CompComplex>(obj);
		auto& children = *ccomplex.children;
#endif // GAME_OBJ_ECS
		for (auto& child : children)
		{
			InsertSceneObj(id, child, root, obj_id);
#ifndef GAME_OBJ_ECS
			auto& casset = child->GetSharedComp<n0::CompAsset>();
			obj_id += casset.GetNodeCount();
#endif // GAME_OBJ_ECS
		}

		Expand(id);
	}
}

void WxSceneTreeCtrl::DeleteSceneObj(const ee0::VariantSet& variants)
{
	ClearALLSelected();

	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err model obj");
	DeleteSceneObj(*obj);
}

void WxSceneTreeCtrl::DeleteSceneObj(const ee0::GameObj& obj)
{
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (Item*)GetItemData(item);
		if (pdata->GetObj() == obj) {
			DeleteEmptyObjToRoot(item);
			return false;
		} else {
			return true;
		}
	});
}

void WxSceneTreeCtrl::ClearSceneObj()
{
	//ClearFocusedItem();
	//DeleteChildren(m_root);

	DeleteAllItems();
	InitRoot();
}

void WxSceneTreeCtrl::ReorderSceneObj(const ee0::VariantSet& variants)
{
	ClearALLSelected();

	auto obj_var = variants.GetVariant("obj");
	GD_ASSERT(obj_var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(obj_var.m_val.pv);
	GD_ASSERT(obj, "err model obj");

	auto up_var = variants.GetVariant("up");
	GD_ASSERT(up_var.m_type == ee0::VT_BOOL, "no var in vars: up");
	bool up = up_var.m_val.bl;

	wxTreeItemId selected;
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (Item*)GetItemData(item);
		if (pdata->GetObj() == *obj) {
			selected = item;
			return false;
		}
		return true;
	});
	GD_ASSERT(selected.IsOk(), "not found");

	ReorderItem(selected, up);
}

bool WxSceneTreeCtrl::ReorderItem(wxTreeItemId item, bool up)
{
	GD_ASSERT(item.IsOk(), "err item");

	wxTreeItemId insert_prev;
	if (up)
	{
		wxTreeItemId prev = GetPrevSibling(item);
		if (!prev.IsOk()) {
			return false;
		}
		insert_prev = GetPrevSibling(prev);
	}
	else
	{
		wxTreeItemId next = GetNextSibling(item);
		if (!next.IsOk()) {
			return false;
		}
		insert_prev = next;
	}

	// old info
	auto pdata = (Item*)GetItemData(item);
	auto old_obj = pdata->GetObj();
	std::string name = GetItemText(item);
	// insert
	wxTreeItemId parent = GetItemParent(item);
	wxTreeItemId new_item;
	if (insert_prev.IsOk()) {
		new_item = InsertItem(parent, insert_prev, name.c_str(), -1, -1, new Item(*pdata));
	} else {
		new_item = InsertItem(parent, 0, name.c_str(), -1, -1, new Item(*pdata));
	}
#ifndef GAME_OBJ_ECS
	bool is_complex = pdata->GetObj()->HasSharedComp<n0::CompComplex>();
#else
	bool is_complex = m_world.HasComponent<e2::CompComplex>(pdata->GetObj());
#endif // GAME_OBJ_ECS
	SetItemBold(new_item, is_complex);
	ExpandAll();
	// copy older's children
	CopyChildrenTree(item, new_item);
	// remove
	DeleteEmptyObjToRoot(item);
	// select
	SelectSceneObj(old_obj, false);

	return true;
}

void WxSceneTreeCtrl::ReorderSelectionToMsg(bool up)
{
	ee0::SelectionSet<ee0::GameObjWithPos> selection;
	wxArrayTreeItemIds items;
	int count = GetSelections(items);
	for (int i = 0; i < count; ++i)
	{
		auto data = (Item*)GetItemData(items[i]);
#ifndef GAME_OBJ_ECS
		selection.Add(ee0::GameObjWithPos(
			data->GetObj(), data->GetRoot(), data->GetObjID()));
#else
		selection.Add(data->GetObj());
#endif // GAME_OBJ_ECS
	}

	if (up) {
		ee2::NodeReorderHelper::UpOneLayer(*m_sub_mgr, selection);
	} else {
		ee2::NodeReorderHelper::DownOneLayer(*m_sub_mgr, selection);
	}
}

void WxSceneTreeCtrl::StagePageChanged(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("new_page");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: new_page");
	GD_ASSERT(var.m_val.pv, "err new_page");

	auto new_page = static_cast<WxStagePage*>(var.m_val.pv);

	m_sub_mgr = new_page->GetSubjectMgr();
	RegisterMsg(*m_sub_mgr);

	m_root_obj = new_page->GetEditedObj();
	RebuildTree(m_root_obj);
}

void WxSceneTreeCtrl::ClearALLSelected()
{
	UnselectAll();
	ClearFocusedItem();

	m_dragged_item.Unset();
}

void WxSceneTreeCtrl::CopyChildrenTree(wxTreeItemId from, wxTreeItemId to)
{
	std::map<wxTreeItemId, wxTreeItemId> old2new;
	old2new.insert(std::make_pair(GetItemParent(from), GetItemParent(to)));
	old2new.insert(std::make_pair(from, to));

	Traverse(from, [&](wxTreeItemId id)->bool
	{
		if (old2new.find(id) != old2new.end()) {
			return true;
		}

		std::string str = GetItemText(id);

		wxTreeItemId old_parent = GetItemParent(id);
		std::string str_p = GetItemText(old_parent);

		auto itr = old2new.find(old_parent);
		GD_ASSERT(itr != old2new.end(), "err id");
		wxTreeItemId new_parent = itr->second;

		std::string str_p_n;
		if (new_parent != m_root) {
			str_p_n = GetItemText(new_parent);
		}

		auto data = (Item*)GetItemData(id);
		std::string name = GetItemText(id);
		wxTreeItemId new_item = AppendItem(new_parent, name.c_str(), -1, -1, new Item(*data));
		ExpandAll();
		old2new.insert(std::make_pair(id, new_item));

		return true;
	});
}

void WxSceneTreeCtrl::MoveSceneObj(wxTreeItemId src, wxTreeItemId dst_parent)
{
	auto src_data = (Item*)GetItemData(src);
	auto src_obj = src_data->GetObj();

	auto dst_parent_data = (Item*)GetItemData(dst_parent);
	auto dst_parent_obj = dst_parent_data->GetObj();

	// todo esc
#ifndef GAME_OBJ_ECS
	// calc world srt transform
	pt2::SRT src_world_srt;
	std::vector<ee0::GameObj> path;
	n0::SceneTreeHelper::GetPathToRoot(src_data->GetRoot(), src_data->GetObjID(), path);
	for (auto& obj : path) {
		auto& ctrans = obj->GetUniqueComp<n2::CompTransform>();
		src_world_srt = src_world_srt * ctrans.GetTrans().GetSRT();
	}

	// set local srt transform
	auto& src_ctrans = src_obj->GetUniqueComp<n2::CompTransform>();
	if (dst_parent_obj)
	{
		auto& dst_ctrans = dst_parent_obj->GetUniqueComp<n2::CompTransform>();
		src_ctrans.SetAngle(*src_obj, src_world_srt.angle - dst_ctrans.GetTrans().GetAngle());
		src_ctrans.SetScale(*src_obj, src_world_srt.scale / dst_ctrans.GetTrans().GetScale());
		src_ctrans.SetPosition(*src_obj, src_world_srt.position - dst_ctrans.GetTrans().GetPosition());
	}
	else
	{
		src_ctrans.SetSRT(*src_obj, src_world_srt);
	}
#endif // GAME_OBJ_ECS

	// remote from old place
	DeleteObjOutside(src);
	CleanRootEmptyChild();

	// insert to new place
	if (GAME_OBJ_VALID(dst_parent_obj))
	{
#ifndef GAME_OBJ_ECS
		auto& ccomplex = dst_parent_obj->GetSharedComp<n0::CompComplex>();
		ccomplex.AddChild(src_obj);

		n2::SceneTreeHelper::UpdateAABB(dst_parent_obj,
			dst_parent_data->GetRoot(), dst_parent_data->GetObjID());
#else
		auto& ccomplex = m_world.AddComponent<e2::CompComplex>(dst_parent_obj);
		ccomplex.children->push_back(src_obj);
#endif // GAME_OBJ_ECS
	}
	else
	{
		ee0::VariantSet vars;

		ee0::Variant var_skip;
		var_skip.m_type = ee0::VT_PVOID;
		var_skip.m_val.pv = static_cast<Observer*>(this);
		vars.SetVariant("skip_observer", var_skip);

		ee0::Variant var_obj;
		var_obj.m_type = ee0::VT_PVOID;
#ifndef GAME_OBJ_ECS
		var_obj.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(src_obj);
#else
		var_obj.m_val.pv = &const_cast<e0::Entity&>(src_obj);
#endif // GAME_OBJ_ECS
		vars.SetVariant("obj", var_obj);

		m_sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_INSERT, vars);
	}
}

void WxSceneTreeCtrl::DeleteEmptyObjToRoot(wxTreeItemId item)
{
	wxTreeItemId curr = item;
	while (curr != m_root)
	{
		auto parent = GetItemParent(curr);
		if (parent == m_root) {
			Delete(curr);
			break;
		}

		auto data = (Item*)GetItemData(parent);
#ifndef GAME_OBJ_ECS
		auto& ccomplex = data->GetObj()->GetSharedComp<n0::CompComplex>();
		if (ccomplex.GetAllChildren().empty())
#else
		auto& ccomplex = m_world.GetComponent<e2::CompComplex>(data->GetObj());
		if (ccomplex.children->empty())
#endif // GAME_OBJ_ECS
		{
			wxTreeItemId old = curr;
			curr = GetItemParent(curr);
			Delete(old);
		}
		else
		{
			Delete(curr);
			break;
		}
	}
}

void WxSceneTreeCtrl::RebuildTree(const ee0::GameObj& obj)
{
	DeleteAllItems();
	InitRoot();

	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& casset = obj->GetSharedComp<n0::CompAsset>();
	casset.Traverse([&](const ee0::GameObj& obj)->bool {
		InsertSceneObj(m_root, obj, obj, 0);
		return true;
	});
#endif // GAME_OBJ_ECS
	Expand(m_root);
}

void WxSceneTreeCtrl::RebuildTree(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err model obj");
	m_root_obj = *obj;
	RebuildTree(m_root_obj);
}

void WxSceneTreeCtrl::ChangeName(const ee0::VariantSet& variants)
{
	auto selected = GetSingleSelected();
	if (!selected.IsOk()) {
		return;
	}

	auto data = (Item*)GetItemData(selected);
	GD_ASSERT(data, "err data");

	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err model obj");

	GD_ASSERT(data->GetObj() == *obj, "err obj");

#ifndef GAME_OBJ_ECS
	auto& name = data->GetObj()->GetUniqueComp<n0::CompIdentity>().GetName();
#else
	std::string name;
	auto& ceditor = m_world.GetComponent<ee0::CompEntityEditor>(data->GetObj());
	if (ceditor.name) {
		name = *ceditor.name;
	}
#endif // GAME_OBJ_ECS
	SetItemText(selected, name);
}

void WxSceneTreeCtrl::SelectUp()
{
	auto selected = GetSingleSelected();
	if (!selected.IsOk()) {
		return;
	}

	wxTreeItemId prev = GetPrevSibling(selected);
	if (prev.IsOk()) {
		SelectItem(prev);
	}
}

void WxSceneTreeCtrl::SelectDown()
{
	auto selected = GetSingleSelected();
	if (!selected.IsOk()) {
		return;
	}

	wxTreeItemId next = GetNextSibling(selected);
	if (next.IsOk()) {
		SelectItem(next);
	}
}

void WxSceneTreeCtrl::SelectLeft()
{
	auto selected = GetSingleSelected();
	if (!selected.IsOk()) {
		return;
	}

	if (ItemHasChildren(selected) && IsExpanded(selected)) {
		Collapse(selected);
	}
	else {
		wxTreeItemId parent = GetItemParent(selected);
		if (parent.IsOk() && parent != m_root) {
			SelectItem(parent);
		}
	}
}

void WxSceneTreeCtrl::SelectRight()
{
	auto selected = GetSingleSelected();
	if (!selected.IsOk()) {
		return;
	}

	if (ItemHasChildren(selected)) {
		if (IsExpanded(selected)) {
			wxTreeItemIdValue cookie;
			wxTreeItemId next = GetFirstChild(selected, cookie);
			if (next.IsOk()) {
				SelectItem(next);
			}
		}
		else {
			Expand(selected);
		}
	}
}

void WxSceneTreeCtrl::DeleteSelectedObj()
{
	auto selected = GetSingleSelected();
	if (!selected.IsOk()) {
		return;
	}

	DeleteObjOutside(selected);
	CleanRootEmptyChild();

	m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

	RebuildTree(m_root_obj);

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxSceneTreeCtrl::DeleteObjOutside(wxTreeItemId item)
{
	auto data = (Item*)GetItemData(item);
	if (GAME_OBJ_VALID(data->GetRoot()) && data->GetObjID() != 0)
	{
		// todo ecs
#ifndef GAME_OBJ_ECS
		wxTreeItemId parent = GetItemParent(item);
		auto pdata = (Item*)GetItemData(parent);
		auto& ccomplex = pdata->GetObj()->GetSharedComp<n0::CompComplex>();
		bool succ = ccomplex.RemoveChild(data->GetObj());
		GD_ASSERT(succ, "fail to remove");

		if (ccomplex.GetAllChildren().empty()) {
			DeleteObjOutside(parent);
		} else {
			n2::SceneTreeHelper::UpdateAABB(pdata->GetObj(), pdata->GetRoot(), pdata->GetObjID());
		}
#endif // GAME_OBJ_ECS
	}
	else
	{
		// remove from root;

		ee0::VariantSet vars;

		ee0::Variant var_skip;
		var_skip.m_type = ee0::VT_PVOID;
		var_skip.m_val.pv = static_cast<Observer*>(this);
		vars.SetVariant("skip_observer", var_skip);

		ee0::Variant var_obj;
		var_obj.m_type = ee0::VT_PVOID;
#ifndef GAME_OBJ_ECS
		var_obj.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(data->GetObj());
#else
		var_obj.m_val.pv = &const_cast<e0::Entity&>(data->GetObj());
#endif // GAME_OBJ_ECS
		vars.SetVariant("obj", var_obj);

		m_sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_DELETE, vars);
	}
}

void WxSceneTreeCtrl::CleanRootEmptyChild()
{
	if (!ItemHasChildren(m_root)) {
		return;
	}

	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(m_root, cookie);
	while (child.IsOk())
	{
		auto data = (Item*)GetItemData(child);
#ifndef GAME_OBJ_ECS
		if (data->GetObj()->HasSharedComp<n0::CompComplex>())
#else
		if (m_world.HasComponent<e2::CompComplex>(data->GetObj()))
#endif // GAME_OBJ_ECS
		{
#ifndef GAME_OBJ_ECS
			bool empty = data->GetObj()->GetSharedComp<n0::CompComplex>().GetAllChildren().empty();
#else
			bool empty = m_world.GetComponent<e2::CompComplex>(data->GetObj()).children->empty();
#endif // GAME_OBJ_ECS
			if (empty)
			{
				ee0::MsgHelper::DeleteNode(*m_sub_mgr, data->GetObj());
				child = GetNextSibling(child);
				continue;
			}
		}

		child = GetNextSibling(child);
	}
}

wxTreeItemId WxSceneTreeCtrl::GetSingleSelected() const
{
	wxArrayTreeItemIds selection;
	int count = GetSelections(selection);
	if (count == 1) {
		return selection[0];
	} else {
		return wxTreeItemId();
	}
}

}