#include "frame/WxSceneTreeCtrl.h"
#include "frame/WxSceneTreeItem.h"
#include "frame/WxStagePage.h"

#include <ee0/SubjectMgr.h>
#include <ee0/CompNodeEditor.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node0/CompAsset.h>
#include <node2/CompComplex.h>

#include <queue>

namespace eone
{


BEGIN_EVENT_TABLE(WxSceneTreeCtrl, wxTreeCtrl)
	EVT_TREE_SEL_CHANGED(ID_SCENE_TREE_CTRL, WxSceneTreeCtrl::OnSelChanged)
	EVT_TREE_END_LABEL_EDIT(ID_SCENE_TREE_CTRL, WxSceneTreeCtrl::OnLabelEdited)
END_EVENT_TABLE()

WxSceneTreeCtrl::WxSceneTreeCtrl(wxWindow* parent, ee0::SubjectMgr& sub_mgr)
	: wxTreeCtrl(parent, ID_SCENE_TREE_CTRL, wxDefaultPosition, wxDefaultSize,
		wxTR_HIDE_ROOT | wxTR_EDIT_LABELS | wxTR_MULTIPLE | wxTR_NO_LINES | wxTR_DEFAULT_STYLE)
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
}

void WxSceneTreeCtrl::OnSelChanged(wxTreeEvent& event)
{
	auto id = event.GetItem();
	if (!id.IsOk() || id == m_root) {
		return;
	}

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

	if (IsSelected(id)) {
		m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_INSERT, vars);
	} else {
		m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_DELETE, vars);
	}
}

void WxSceneTreeCtrl::OnLabelEdited(wxTreeEvent& event)
{

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

void WxSceneTreeCtrl::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	wxTreeItemId parent = GetFocusedItem();
	if (!parent.IsOk()) {
		parent = m_root;
	}
	while (parent != m_root)
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(parent);
		auto& pnode = pdata->GetNode();
		if (pdata->GetNode()->HasSharedComp<n2::CompComplex>()) {
			break;
		} else {
			parent = GetItemParent(parent);
		}
	}

	n0::SceneNodePtr root;
	size_t node_id;
	if (parent == m_root) 
	{
		root = *node;
		node_id = 0;
	}
	else
	{
		auto pdata = (WxSceneTreeItem*)GetItemData(parent);
		root = pdata->GetRoot();
		// todo
		node_id = 0;
	}
	InsertSceneNode(parent, *node, root, node_id);

	if (parent != m_root) {
		Expand(parent);
	}
}

void WxSceneTreeCtrl::InsertSceneNode(wxTreeItemId parent, const n0::SceneNodePtr& node,
	                                  const n0::SceneNodePtr& root, size_t node_id)
{
	auto item = new WxSceneTreeItem(node, root, node_id);
	auto& ceditor = node->GetUniqueComp<ee0::CompNodeEditor>();
	wxTreeItemId id = InsertItem(parent, 0, ceditor.GetName());
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
			Delete(item);
		}
		return true;
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
	ExpandAll();
	// font style
	auto node = pdata->GetNode();
	if (node->HasSharedComp<n2::CompComplex>()) {
		SetItemBold(new_item, true);
	}
	// copy older's children
	CopyChildren(item, new_item);
	// remove
	Delete(item);
	// sort
//	ReorderSprites();
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
}

void WxSceneTreeCtrl::CopyChildren(wxTreeItemId from, wxTreeItemId to)
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

		std::map<wxTreeItemId, wxTreeItemId>::iterator itr
			= old2new.find(old_parent);
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

}