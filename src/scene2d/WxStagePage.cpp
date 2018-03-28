#include "scene2d/WxStagePage.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompComplex.h>

namespace eone
{
namespace scene2d
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node)
	: eone::WxStagePage(parent, node, SUB_WND_PREVIEW)
{
	m_messages.push_back(ee0::MSG_INSERT_SCENE_NODE);
	m_messages.push_back(ee0::MSG_DELETE_SCENE_NODE);
	m_messages.push_back(ee0::MSG_CLEAR_SCENE_NODE);
	m_messages.push_back(ee0::MSG_REORDER_SCENE_NODE);

	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(library, this));
	}
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	bool dirty = false;
	switch (msg)
	{
	case ee0::MSG_INSERT_SCENE_NODE:
		dirty = InsertSceneNode(variants);
		break;
	case ee0::MSG_DELETE_SCENE_NODE:
		dirty = DeleteSceneNode(variants);
		break;
	case ee0::MSG_CLEAR_SCENE_NODE:
		dirty = ClearSceneNode();
		break;
	case ee0::MSG_REORDER_SCENE_NODE:
		dirty = ReorderSceneNode(variants);
		break;
	}

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
	                       const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY) {
		m_node->GetSharedComp<n2::CompComplex>().Traverse(func, inverse);
		return;
	}
	
	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		func(m_node);
		break;
	default:
		m_node->GetSharedComp<n2::CompComplex>().Traverse(func, inverse);
	}
}

const n0::NodeSharedComp& WxStagePage::GetEditedNodeComp() const 
{
	return m_node->GetSharedComp<n2::CompComplex>();
}

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val, 
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
//	val.AddMember("camera", "2d", alloc);
}

bool WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto& ccomplex = m_node->GetSharedComp<n2::CompComplex>();
	ccomplex.AddChild(*node);

	return true;
}

bool WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto& ccomplex = m_node->GetSharedComp<n2::CompComplex>();
	return ccomplex.RemoveChild(*node);
}

bool WxStagePage::ClearSceneNode()
{
	auto& ccomplex = m_node->GetSharedComp<n2::CompComplex>();
	bool dirty = !ccomplex.GetAllChildren().empty();
	ccomplex.RemoveAllChildren();
	return dirty;
}

bool WxStagePage::ReorderSceneNode(const ee0::VariantSet& variants)
{
	auto node_var = variants.GetVariant("node");
	GD_ASSERT(node_var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(node_var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto up_var = variants.GetVariant("up");
	GD_ASSERT(up_var.m_type == ee0::VT_BOOL, "no var in vars: up");
	bool up = up_var.m_val.bl;

	auto& ccomplex = m_node->GetSharedComp<n2::CompComplex>();
	std::vector<n0::SceneNodePtr> all_nodes = ccomplex.GetAllChildren();
	if (all_nodes.empty()) {
		return false;
	}

	int idx = -1;
	for (int i = 0, n = all_nodes.size(); i < n; ++i) 
	{
		if (all_nodes[i] == *node) {
			idx = i;
			break;
		}
	}

	GD_ASSERT(idx >= 0, "not find");

	if (up && idx != all_nodes.size() - 1)
	{
		std::swap(all_nodes[idx], all_nodes[idx + 1]);
		ccomplex.SetChildren(all_nodes);
		return true;
	}
	else if (!up && idx != 0)
	{
		std::swap(all_nodes[idx], all_nodes[idx - 1]);
		ccomplex.SetChildren(all_nodes);
		return true;
	}

	return false;
}

}
}