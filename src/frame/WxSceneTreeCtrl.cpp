#include "frame/WxSceneTreeCtrl.h"
#include "frame/WxSceneTreeItem.h"
#include "frame/WxStagePage.h"
#include "frame/MessageID.h"

#include <ee0/SubjectMgr.h>
#include <ee0/CompNodeEditor.h>
#include <ee0/MsgHelper.h>
#include <ee2/NodeReorderHelper.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node0/CompAsset.h>
#include <node0/SceneTreeHelper.h>
#include <node2/CompComplex.h>
#include <node2/CompTransform.h>
#include <node2/SceneTreeHelper.h>

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
	                             const ee0::GameObj& root_obj)
	: wxTreeCtrl(parent, ID_SCENE_TREE_CTRL, wxDefaultPosition, wxDefaultSize,
		/*wxTR_HIDE_ROOT | */wxTR_EDIT_LABELS | wxTR_MULTIPLE | wxTR_NO_LINES | wxTR_DEFAULT_STYLE)
	, m_sub_mgr(sub_mgr)
	, m_root_obj(root_obj)
	, m_disable_select(false)
{
	SetBackgroundColour(wxColour(229, 229, 229));

	InitRoot();
	RegisterMsg(*m_sub_mgr);
}

void WxSceneTreeCtrl::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_INSERT_SCENE_NODE:
		InsertSceneObj(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
		DeleteSceneObj(variants);
		break;
	case ee0::MSG_CLEAR_SCENE_NODE:
		ClearSceneObj();
		break;
	case ee0::MSG_REORDER_SCENE_NODE:
		ReorderSceneObj(variants);
		break;

	case ee0::MSG_GAME_OBJ_SELECTION_INSERT:
		if (!m_disable_select) {
			SelectSceneObj(variants);
		}
		break;
	case ee0::MSG_GAME_OBJ_SELECTION_DELETE:
		UnselectSceneObj(variants);
		break;
	case ee0::MSG_GAME_OBJ_SELECTION_CLEAR:
		ClearALLSelected();
		break;
	case ee0::MSG_STAGE_PAGE_CHANGED:
		StagePageChanged(variants);
		break;

	case ee0::MSG_UPDATE_GAME_OBJ_NAME:
		ChangeName(variants);
		break;

	case MSG_TREE_PANEL_REBUILD:
		RebuildTree(variants);
		break;
	}
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
	SetItemData(m_root, new WxSceneTreeItem(nullptr, nullptr, -1));
	SetItemBold(m_root, true);
}

void WxSceneTreeCtrl::RegisterMsg(ee0::SubjectMgr& sub_mgr)
{
	sub_mgr.RegisterObserver(ee0::MSG_INSERT_SCENE_NODE, this);
	sub_mgr.RegisterObserver(ee0::MSG_DELETE_SCENE_NODE, this);
	sub_mgr.RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);
	sub_mgr.RegisterObserver(ee0::MSG_REORDER_SCENE_NODE, this);

	sub_mgr.RegisterObserver(ee0::MSG_GAME_OBJ_SELECTION_INSERT, this);
	sub_mgr.RegisterObserver(ee0::MSG_GAME_OBJ_SELECTION_DELETE, this);
	sub_mgr.RegisterObserver(ee0::MSG_GAME_OBJ_SELECTION_CLEAR, this);
	sub_mgr.RegisterObserver(ee0::MSG_STAGE_PAGE_CHANGED, this);

	sub_mgr.RegisterObserver(ee0::MSG_UPDATE_GAME_OBJ_NAME, this);

	sub_mgr.RegisterObserver(MSG_TREE_PANEL_REBUILD, this);
}

void WxSceneTreeCtrl::OnSelChanged(wxTreeEvent& event)
{
	auto id = event.GetItem();
	if (!id.IsOk() || id == m_root) {
		return;
	}

	auto data = (WxSceneTreeItem*)GetItemData(id);
	auto& obj = data->GetObj();
	GD_ASSERT(obj, "err scene obj.");

	ee0::VariantSet vars;

	ee0::Variant var_obj;
	var_obj.m_type = ee0::VT_PVOID;
	var_obj.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(obj);
	vars.SetVariant("obj", var_obj);

	ee0::Variant var_root;
	var_root.m_type = ee0::VT_PVOID;
	var_root.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(data->GetRoot());
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

		m_sub_mgr->NotifyObservers(ee0::MSG_GAME_OBJ_SELECTION_INSERT, vars);
	} 
	else 
	{
		m_sub_mgr->NotifyObservers(ee0::MSG_GAME_OBJ_SELECTION_DELETE, vars);
	}
}

void WxSceneTreeCtrl::OnLabelEdited(wxTreeEvent& event)
{
	wxTreeItemId edited_item = event.GetItem();
	auto data_dst = (WxSceneTreeItem*)GetItemData(edited_item);
	if (!data_dst) {
		return;
	}

	auto& ceditor = data_dst->GetObj()->GetUniqueComp<ee0::CompNodeEditor>();
	ceditor.SetName(event.GetLabel().ToStdString());
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
	auto data_dst = (WxSceneTreeItem*)GetItemData(new_item_parent);
	if (!data_dst) {
		return;
	}

	wxTreeItemId new_item;

	// old info
	auto data_src = (WxSceneTreeItem*)GetItemData(old_item);
	std::string name = GetItemText(old_item);
	auto old_obj = data_src->GetObj();

	// move to root
	if (data_dst->GetObj() == nullptr)
	{
		new_item = InsertItem(new_item_parent, 0, name.c_str(), -1, -1,
			new WxSceneTreeItem(data_src->GetObj(), data_src->GetObj(), 0));
	}
	else
	{
		if (!data_dst->GetObj()->HasSharedComp<n2::CompComplex>()) {
			return;
		}

		auto& dst_casset = data_dst->GetObj()->GetSharedComp<n0::CompAsset>();
		int new_obj_id = data_dst->GetObjID() + dst_casset.GetNodeCount();
		new_item = InsertItem(new_item_parent, 0, name.c_str(), -1, -1,
			new WxSceneTreeItem(data_src->GetObj(), data_dst->GetRoot(), new_obj_id));
	}

	SetItemBold(new_item, data_src->GetObj()->HasSharedComp<n2::CompComplex>());
	// move scene tree
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
	GD_ASSERT(obj, "err scene obj");

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
	GD_ASSERT(obj, "err scene obj");

	UnselectSceneObj(*obj);
}

void WxSceneTreeCtrl::SelectSceneObj(const ee0::GameObj& obj, bool multiple)
{
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(item);
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
		auto pdata = (WxSceneTreeItem*)GetItemData(item);
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
	GD_ASSERT(obj, "err scene obj");

	InsertSceneObj(m_root, *obj, *obj, 0);

	Expand(m_root);
}

void WxSceneTreeCtrl::InsertSceneObj(wxTreeItemId parent, const ee0::GameObj& obj,
	                                  const ee0::GameObj& root, size_t obj_id)
{
	auto item = new WxSceneTreeItem(obj, root, obj_id);
	auto& ceditor = obj->GetUniqueComp<ee0::CompNodeEditor>();
	wxTreeItemId id = InsertItem(parent, 0, ceditor.GetName());
	SetItemBold(id, obj->HasSharedComp<n2::CompComplex>());
	SetItemData(id, item);
	if (obj->HasSharedComp<n2::CompComplex>())
	{
		auto& ccomplex = obj->GetSharedComp<n2::CompComplex>();
		auto& children = ccomplex.GetAllChildren();
		obj_id = obj_id + 1;
		for (auto& child : children) 
		{
			InsertSceneObj(id, child, root, obj_id);
			auto& casset = child->GetSharedComp<n0::CompAsset>();
			obj_id += casset.GetNodeCount();
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
	GD_ASSERT(obj, "err scene obj");
	DeleteSceneObj(*obj);
}

void WxSceneTreeCtrl::DeleteSceneObj(const ee0::GameObj& obj)
{
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(item);
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
	GD_ASSERT(obj, "err scene obj");

	auto up_var = variants.GetVariant("up");
	GD_ASSERT(up_var.m_type == ee0::VT_BOOL, "no var in vars: up");
	bool up = up_var.m_val.bl;

	wxTreeItemId selected;
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(item);
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
	auto pdata = (WxSceneTreeItem*)GetItemData(item);
	auto old_obj = pdata->GetObj();
	std::string name = GetItemText(item);
	// insert
	wxTreeItemId parent = GetItemParent(item);
	wxTreeItemId new_item;
	if (insert_prev.IsOk()) {
		new_item = InsertItem(parent, insert_prev, name.c_str(), -1, -1, new WxSceneTreeItem(*pdata));
	} else {
		new_item = InsertItem(parent, 0, name.c_str(), -1, -1, new WxSceneTreeItem(*pdata));
	}
	SetItemBold(new_item, pdata->GetObj()->HasSharedComp<n2::CompComplex>());
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
	ee0::SelectionSet<n0::NodeWithPos> selection;
	wxArrayTreeItemIds items;
	int count = GetSelections(items);
	for (int i = 0; i < count; ++i) 
	{
		auto data = (WxSceneTreeItem*)GetItemData(items[i]);
		selection.Add(n0::NodeWithPos(
			data->GetObj(), data->GetRoot(), data->GetObjID()));
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

		auto data = (WxSceneTreeItem*)GetItemData(id);
		std::string name = GetItemText(id);
		wxTreeItemId new_item = AppendItem(new_parent, name.c_str(), -1, -1, new WxSceneTreeItem(*data));
		ExpandAll();
		old2new.insert(std::make_pair(id, new_item));

		return true;
	});
}

void WxSceneTreeCtrl::MoveSceneObj(wxTreeItemId src, wxTreeItemId dst_parent)
{
	auto src_data = (WxSceneTreeItem*)GetItemData(src);
	auto src_obj = src_data->GetObj();

	auto dst_parent_data = (WxSceneTreeItem*)GetItemData(dst_parent);
	auto dst_parent_obj = dst_parent_data->GetObj();

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

	// remote from old place
	DeleteObjOutside(src);
	CleanRootEmptyChild();

	// insert to new place
	if (dst_parent_obj)
	{
		auto& ccomplex = dst_parent_obj->GetSharedComp<n2::CompComplex>();
		ccomplex.AddChild(src_obj);

		n2::SceneTreeHelper::UpdateAABB(dst_parent_obj, 
			dst_parent_data->GetRoot(), dst_parent_data->GetObjID());
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
		var_obj.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(src_obj);
		vars.SetVariant("obj", var_obj);

		m_sub_mgr->NotifyObservers(ee0::MSG_INSERT_SCENE_NODE, vars);
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

		auto data = (WxSceneTreeItem*)GetItemData(parent);
		auto& ccomplex = data->GetObj()->GetSharedComp<n2::CompComplex>();
		if (ccomplex.GetAllChildren().empty()) 
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

	auto& casset = obj->GetSharedComp<n0::CompAsset>();
	casset.Traverse([&](const ee0::GameObj& obj)->bool {
		InsertSceneObj(m_root, obj, obj, 0);
		return true;
	});
	Expand(m_root);
}

void WxSceneTreeCtrl::RebuildTree(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");
	m_root_obj = *obj;
	RebuildTree(m_root_obj);
}

void WxSceneTreeCtrl::ChangeName(const ee0::VariantSet& variants)
{
	auto selected = GetSingleSelected();
	if (!selected.IsOk()) {
		return;
	}

	auto data = (WxSceneTreeItem*)GetItemData(selected);
	GD_ASSERT(data, "err data");
	
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	GD_ASSERT(data->GetObj() == *obj, "err obj");

	auto& ceditor = data->GetObj()->GetUniqueComp<ee0::CompNodeEditor>();
	SetItemText(selected, ceditor.GetName());
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
	
	m_sub_mgr->NotifyObservers(ee0::MSG_GAME_OBJ_SELECTION_CLEAR);

	RebuildTree(m_root_obj);

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxSceneTreeCtrl::DeleteObjOutside(wxTreeItemId item)
{
	auto data = (WxSceneTreeItem*)GetItemData(item);
	if (data->GetRoot() && data->GetObjID() != 0)
	{
		wxTreeItemId parent = GetItemParent(item);
		auto pdata = (WxSceneTreeItem*)GetItemData(parent);
		auto& ccomplex = pdata->GetObj()->GetSharedComp<n2::CompComplex>();
		bool succ = ccomplex.RemoveChild(data->GetObj());
		GD_ASSERT(succ, "fail to remove");

		if (ccomplex.GetAllChildren().empty()) {
			DeleteObjOutside(parent);
		} else {
			n2::SceneTreeHelper::UpdateAABB(pdata->GetObj(), pdata->GetRoot(), pdata->GetObjID());
		}
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
		var_obj.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(data->GetObj());
		vars.SetVariant("obj", var_obj);

		m_sub_mgr->NotifyObservers(ee0::MSG_DELETE_SCENE_NODE, vars);
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
		auto data = (WxSceneTreeItem*)GetItemData(child);
		if (data->GetObj()->HasSharedComp<n2::CompComplex>())
		{
			auto& ccomplex = data->GetObj()->GetSharedComp<n2::CompComplex>();
			if (ccomplex.GetAllChildren().empty()) 
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