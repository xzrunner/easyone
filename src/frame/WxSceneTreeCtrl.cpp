#include "frame/WxSceneTreeCtrl.h"
#include "frame/WxSceneTreeItem.h"
#include "frame/WxStagePage.h"

#include <ee0/SubjectMgr.h>
#include <ee0/CompNodeEditor.h>
#include <ee0/MsgHelper.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node0/CompAsset.h>
#include <node2/CompComplex.h>
#include <node2/CompTransform.h>

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

WxSceneTreeCtrl::WxSceneTreeCtrl(wxWindow* parent, ee0::SubjectMgr& sub_mgr)
	: wxTreeCtrl(parent, ID_SCENE_TREE_CTRL, wxDefaultPosition, wxDefaultSize,
		/*wxTR_HIDE_ROOT | */wxTR_EDIT_LABELS | wxTR_MULTIPLE | wxTR_NO_LINES | wxTR_DEFAULT_STYLE)
	, m_sub_mgr(&sub_mgr)
{
	SetBackgroundColour(wxColour(229, 229, 229));

	InitRoot();
	RegisterMsg(*m_sub_mgr);
}

void WxSceneTreeCtrl::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	switch (msg)
	{
	case ee0::MSG_INSERT_SCENE_NODE:
		InsertSceneNode(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
		DeleteSceneNode(variants);
		break;
	case ee0::MSG_CLEAR_SCENE_NODE:
		ClearSceneNode();
		break;
	case ee0::MSG_REORDER_SCENE_NODE:
		ReorderSceneNode(variants);
		break;

	case ee0::MSG_NODE_SELECTION_INSERT:
		SelectSceneNode(variants);
		break;
	case ee0::MSG_NODE_SELECTION_DELETE:
		UnselectSceneNode(variants);
		break;
	case ee0::MSG_NODE_SELECTION_CLEAR:
		ClearALLSelected();
		break;
	case ee0::MSG_STAGE_PAGE_CHANGED:
		DeleteAllItems();
		InitRoot();
		StagePageChanged(variants);
		break;

	case ee0::MSG_UPDATE_NODE_NAME:
		ChangeName(variants);
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

	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_DELETE, this);
	sub_mgr.RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
	sub_mgr.RegisterObserver(ee0::MSG_STAGE_PAGE_CHANGED, this);

	sub_mgr.RegisterObserver(ee0::MSG_UPDATE_NODE_NAME, this);
}

void WxSceneTreeCtrl::OnSelChanged(wxTreeEvent& event)
{
	auto id = event.GetItem();
	if (!id.IsOk() || id == m_root) {
		return;
	}

	m_selected_item = id;

	auto data = (WxSceneTreeItem*)GetItemData(id);
	auto& node = data->GetNode();
	GD_ASSERT(node, "err scene node.");

	ee0::VariantSet vars;

	ee0::Variant var_node;
	var_node.m_type = ee0::VT_PVOID;
	var_node.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(node);
	vars.SetVariant("node", var_node);

	ee0::Variant var_root;
	var_root.m_type = ee0::VT_PVOID;
	var_root.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(data->GetRoot());
	vars.SetVariant("root", var_root);

	ee0::Variant var_id;
	var_id.m_type = ee0::VT_ULONG;
	var_id.m_val.ul = data->GetNodeID();
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
	auto data_dst = (WxSceneTreeItem*)GetItemData(edited_item);
	if (!data_dst) {
		return;
	}

	auto& ceditor = data_dst->GetNode()->GetUniqueComp<ee0::CompNodeEditor>();
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

	// move to root
	if (data_dst->GetNode() == nullptr)
	{
		new_item = InsertItem(new_item_parent, 0, name.c_str(), -1, -1,
			new WxSceneTreeItem(data_src->GetNode(), data_dst->GetRoot(), 0));
	}
	else
	{
		if (!data_dst->GetNode()->HasSharedComp<n2::CompComplex>()) {
			return;
		}

		auto& dst_casset = data_dst->GetNode()->GetSharedComp<n0::CompAsset>();
		int new_node_id = data_dst->GetNodeID() + dst_casset.GetNodeCount();
		new_item = InsertItem(new_item_parent, 0, name.c_str(), -1, -1,
			new WxSceneTreeItem(data_src->GetNode(), data_dst->GetRoot(), new_node_id));
	}

	SetItemBold(new_item, data_src->GetNode()->HasSharedComp<n2::CompComplex>());
	ExpandAll();
	// move scene tree
	MoveSceneNode(old_item, data_dst->GetNode());
	// update node id
	UpdateTreeNodeID(new_item);
	UpdateTreeNodeIDToRoot(old_item);
	// copy older's children
	CopyChildrenTree(old_item, new_item);
	// remove
	DeleteEmptyNodeToRoot(old_item);
	// set selection
	m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	SelectItem(new_item);
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
		ReorderItem(m_selected_item, true);
		break;
	case WXK_PAGEDOWN:
		ReorderItem(m_selected_item, false);
		break;
	case WXK_DELETE:
		DeleteSelectedNode();
		break;
	}
}

void WxSceneTreeCtrl::SelectSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	bool multiple = false;
	auto var_multi = variants.GetVariant("multiple");
	if (var_multi.m_type == ee0::VT_BOOL && var_multi.m_val.bl) {
		multiple = true;
	}

	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(item);
		if (pdata->GetNode() == *node) {
			SelectItem(item, !multiple);
			return false;
		} else {
			return true;
		}
	});
}

void WxSceneTreeCtrl::UnselectSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(item);
		if (pdata->GetNode() == *node) {
			UnselectItem(item);
			return false;
		} else {
			return true;
		}
	});
}

// insert to root
void WxSceneTreeCtrl::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	InsertSceneNode(m_root, *node, *node, 0);

	Expand(m_root);
}

//// insert to selected complex
//void WxSceneTreeCtrl::InsertSceneNode(const ee0::VariantSet& variants)
//{
//	auto var = variants.GetVariant("node");
//	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
//	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
//	GD_ASSERT(node, "err scene node");
//
//	wxTreeItemId parent = GetFocusedItem();
//	if (!parent.IsOk()) {
//		parent = m_root;
//	}
//	while (parent != m_root)
//	{
//		auto pdata = (WxSceneTreeItem*)GetItemData(parent);
//		auto& pnode = pdata->GetNode();
//		if (pdata->GetNode()->HasSharedComp<n2::CompComplex>()) {
//			break;
//		} else {
//			parent = GetItemParent(parent);
//		}
//	}
//
//	n0::SceneNodePtr root;
//	size_t node_id;
//	if (parent == m_root) 
//	{
//		root = *node;
//		node_id = 0;
//	}
//	else
//	{
//		auto pdata = (WxSceneTreeItem*)GetItemData(parent);
//		root = pdata->GetRoot();
//		// todo
//		node_id = 0;
//	}
//	InsertSceneNode(parent, *node, root, node_id);
//
//	if (parent != m_root) {
//		Expand(parent);
//	}
//}

void WxSceneTreeCtrl::InsertSceneNode(wxTreeItemId parent, const n0::SceneNodePtr& node,
	                                  const n0::SceneNodePtr& root, size_t node_id)
{
	auto item = new WxSceneTreeItem(node, root, node_id);
	auto& ceditor = node->GetUniqueComp<ee0::CompNodeEditor>();
	wxTreeItemId id = InsertItem(parent, 0, ceditor.GetName());
	SetItemBold(id, node->HasSharedComp<n2::CompComplex>());
	SetItemData(id, item);
	if (node->HasSharedComp<n2::CompComplex>())
	{
		auto& ccomplex = node->GetSharedComp<n2::CompComplex>();
		auto& children = ccomplex.GetAllChildren();
		node_id = node_id + 1;
		for (auto& child : children) 
		{
			InsertSceneNode(id, child, root, node_id);
			auto& casset = child->GetSharedComp<n0::CompAsset>();
			node_id += casset.GetNodeCount();
		}
	}
}

void WxSceneTreeCtrl::DeleteSceneNode(const ee0::VariantSet& variants)
{
	ClearALLSelected();

	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");
	DeleteSceneNode(*node);
}

void WxSceneTreeCtrl::DeleteSceneNode(const n0::SceneNodePtr& node)
{
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(item);
		if (pdata->GetNode() == node) {
			DeleteEmptyNodeToRoot(item);
			return false;
		} else {
			return true;
		}
	});
}

void WxSceneTreeCtrl::ClearSceneNode()
{
	//ClearFocusedItem();
	//DeleteChildren(m_root);

	DeleteAllItems();
	InitRoot();
}

void WxSceneTreeCtrl::ReorderSceneNode(const ee0::VariantSet& variants)
{
	ClearALLSelected();

	auto node_var = variants.GetVariant("node");
	GD_ASSERT(node_var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(node_var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto up_var = variants.GetVariant("up");
	GD_ASSERT(up_var.m_type == ee0::VT_BOOL, "no var in vars: up");
	bool up = up_var.m_val.bl;

	wxTreeItemId selected;
	Traverse(m_root, [&](wxTreeItemId item)->bool
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(item);
		if (pdata->GetNode() == *node) {
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
	std::string name = GetItemText(item);
	// insert
	wxTreeItemId parent = GetItemParent(item);
	wxTreeItemId new_item;
	if (insert_prev.IsOk()) {
		new_item = InsertItem(parent, insert_prev, name.c_str(), -1, -1, new WxSceneTreeItem(*pdata));
	} else {
		new_item = InsertItem(parent, 0, name.c_str(), -1, -1, new WxSceneTreeItem(*pdata));
	}
	SetItemBold(new_item, pdata->GetNode()->HasSharedComp<n2::CompComplex>());
	ExpandAll();
	// copy older's children
	CopyChildrenTree(item, new_item);
	// remove
	DeleteEmptyNodeToRoot(item);
	// set selection
	SelectItem(new_item);

	return true;
}

void WxSceneTreeCtrl::StagePageChanged(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("new_page");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: new_page");
	GD_ASSERT(var.m_val.pv, "err new_page");
	auto new_page = static_cast<WxStagePage*>(var.m_val.pv);
	m_sub_mgr = &new_page->GetSubjectMgr();

	RegisterMsg(*m_sub_mgr);
}

void WxSceneTreeCtrl::ClearALLSelected()
{
	UnselectAll();
	ClearFocusedItem();

	m_dragged_item.Unset();
	m_selected_item.Unset();
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

void WxSceneTreeCtrl::MoveSceneNode(wxTreeItemId src, const n0::SceneNodePtr& dst_parent)
{
	auto src_data = (WxSceneTreeItem*)GetItemData(src);
	auto src_node = src_data->GetNode();
	auto src_root = src_data->GetRoot();
	auto src_node_id = src_data->GetNodeID();

	// calc world srt transform
	pt2::SRT src_world_srt;
	if (src_root && src_node_id != 0)
	{
		GD_ASSERT(src_node_id > 0, "err id");
		size_t src_id = src_node_id;

		size_t curr_id = 0;
		auto curr_node = src_root;
		auto& ctrans = curr_node->GetUniqueComp<n2::CompTransform>();
		src_world_srt = ctrans.GetTrans().GetSRT();
		while (curr_id != src_id)
		{
			auto& casset = curr_node->GetSharedComp<n0::CompAsset>();
			GD_ASSERT(src_id > curr_id && src_id < curr_id + casset.GetNodeCount(), "err id");
			curr_id += 1;
			casset.Traverse([&](const n0::SceneNodePtr& node)->bool
			{
				auto& casset = node->GetSharedComp<n0::CompAsset>();
				if (src_id == curr_id)
				{
					curr_node = node;
					auto& ctrans = curr_node->GetUniqueComp<n2::CompTransform>();
					src_world_srt = src_world_srt * ctrans.GetTrans().GetSRT();
					return false;
				}
				else if (src_id < curr_id + casset.GetNodeCount()) 
				{
					curr_node = node;
					auto& ctrans = curr_node->GetUniqueComp<n2::CompTransform>();
					src_world_srt = src_world_srt * ctrans.GetTrans().GetSRT();
					return false;
				} 
				else 
				{
					curr_id += casset.GetNodeCount();
					return true;
				}
			});
		}
	}
	else
	{
		src_world_srt = src_node->GetUniqueComp<n2::CompTransform>().GetTrans().GetSRT();
	}

	// set local srt transform
	auto& src_ctrans = src_node->GetUniqueComp<n2::CompTransform>();
	if (dst_parent)
	{
		auto& dst_ctrans = dst_parent->GetUniqueComp<n2::CompTransform>();
		src_ctrans.SetAngle(*src_node, src_world_srt.angle - dst_ctrans.GetTrans().GetAngle());
		src_ctrans.SetScale(*src_node, src_world_srt.scale / dst_ctrans.GetTrans().GetScale());
		src_ctrans.SetPosition(*src_node, src_world_srt.position - dst_ctrans.GetTrans().GetPosition());
	}
	else
	{
		src_ctrans.SetSRT(*src_node, src_world_srt);
	}

	// remote from old place
	DeleteNodeOutside(src);

	// insert to new place
	if (dst_parent)
	{
		auto& ccomplex = dst_parent->GetSharedComp<n2::CompComplex>();
		ccomplex.AddChild(src_node);
	}
	else
	{
		ee0::VariantSet vars;

		ee0::Variant var_skip;
		var_skip.m_type = ee0::VT_PVOID;
		var_skip.m_val.pv = static_cast<Observer*>(this);
		vars.SetVariant("skip_observer", var_skip);

		ee0::Variant var_node;
		var_node.m_type = ee0::VT_PVOID;
		var_node.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(src_node);
		vars.SetVariant("node", var_node);

		m_sub_mgr->NotifyObservers(ee0::MSG_INSERT_SCENE_NODE, vars);
	}
}

void WxSceneTreeCtrl::UpdateTreeNodeID(wxTreeItemId root)
{
	auto data = (WxSceneTreeItem*)GetItemData(root);
	if (!data->GetRoot()) {
		return;
	}

	int curr_id = -1;
	Traverse(root, [&](wxTreeItemId id)->bool
	{
		auto data = (WxSceneTreeItem*)GetItemData(id);
		if (id == root) {
			curr_id = data->GetNodeID() + 1;
		} else {
			data->SetNodeID(curr_id++);
		}
		return true;
	});
}

void WxSceneTreeCtrl::UpdateTreeNodeIDToRoot(wxTreeItemId item)
{
	wxTreeItemId curr = item;
	while (curr.IsOk()) {
		UpdateTreeNodeID(curr);
		curr = GetItemParent(curr);
	}
}

void WxSceneTreeCtrl::DeleteEmptyNodeToRoot(wxTreeItemId item)
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
		auto& ccomplex = data->GetNode()->GetSharedComp<n2::CompComplex>();
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

void WxSceneTreeCtrl::RebuildAllTree()
{
	ClearALLSelected();

	if (!ItemHasChildren(m_root)) {
		return;
	}

	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(m_selected_item, cookie);
	while (child.IsOk()) 
	{
		RebuildTree(child);
		child = GetNextSibling(child);
	}
}

// todo
void WxSceneTreeCtrl::RebuildTree(wxTreeItemId item)
{
	auto data = (WxSceneTreeItem*)GetItemData(item);
	if (data->GetNode()->HasSharedComp<n2::CompComplex>())
	{
//		auto& ccomplex = data->GetNode()
	}
	else
	{
		
	}
}

void WxSceneTreeCtrl::ChangeName(const ee0::VariantSet& variants)
{
	GD_ASSERT(m_selected_item.IsOk(), "err item");

	auto data = (WxSceneTreeItem*)GetItemData(m_selected_item);
	GD_ASSERT(data, "err data");
	
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	GD_ASSERT(data->GetNode() == *node, "err node");

	auto& ceditor = data->GetNode()->GetUniqueComp<ee0::CompNodeEditor>();
	SetItemText(m_selected_item, ceditor.GetName());
}

void WxSceneTreeCtrl::SelectUp()
{
	if (!m_selected_item.IsOk()) {
		return;
	}

	wxTreeItemId prev = GetPrevSibling(m_selected_item);
	if (prev.IsOk()) {
		SelectItem(prev);
	}
}

void WxSceneTreeCtrl::SelectDown()
{
	if (!m_selected_item.IsOk()) {
		return;
	}

	wxTreeItemId next = GetNextSibling(m_selected_item);
	if (next.IsOk()) {
		SelectItem(next);
	}
}

void WxSceneTreeCtrl::SelectLeft()
{
	if (!m_selected_item.IsOk()) {
		return;
	}

	if (ItemHasChildren(m_selected_item) && IsExpanded(m_selected_item)) {
		Collapse(m_selected_item);
	}
	else {
		wxTreeItemId parent = GetItemParent(m_selected_item);
		if (parent.IsOk() && parent != m_root) {
			SelectItem(parent);
		}
	}
}

void WxSceneTreeCtrl::SelectRight()
{
	if (!m_selected_item.IsOk()) {
		return;
	}

	if (ItemHasChildren(m_selected_item)) {
		if (IsExpanded(m_selected_item)) {
			wxTreeItemIdValue cookie;
			wxTreeItemId next = GetFirstChild(m_selected_item, cookie);
			if (next.IsOk()) {
				SelectItem(next);
			}
		}
		else {
			Expand(m_selected_item);
		}
	}
}

void WxSceneTreeCtrl::DeleteSelectedNode()
{
	if (!m_selected_item.IsOk()) {
		return;
	}

	DeleteNodeOutside(m_selected_item);

	// 1. update path to root (other path might need update)
	//UpdateTreeNodeIDToRoot(m_selected_item);
	//DeleteEmptyNodeToRoot(m_selected_item);
	//m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

	// 2. update all
	RebuildAllTree();
}

void WxSceneTreeCtrl::DeleteNodeOutside(wxTreeItemId item)
{
	auto data = (WxSceneTreeItem*)GetItemData(item);
	if (data->GetRoot() && data->GetNodeID() != 0)
	{
		wxTreeItemId parent = GetItemParent(item);
		auto pdata = (WxSceneTreeItem*)GetItemData(parent);
		auto& ccomplex = pdata->GetNode()->GetSharedComp<n2::CompComplex>();
		bool succ = ccomplex.RemoveChild(data->GetNode());
		GD_ASSERT(succ, "fail to remove");

		if (ccomplex.GetAllChildren().empty()) {
			DeleteNodeOutside(parent);
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

		ee0::Variant var_node;
		var_node.m_type = ee0::VT_PVOID;
		var_node.m_val.pv = &std::const_pointer_cast<n0::SceneNode>(data->GetNode());
		vars.SetVariant("node", var_node);

		m_sub_mgr->NotifyObservers(ee0::MSG_DELETE_SCENE_NODE, vars);
	}
}

}