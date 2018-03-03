#include "mask/WxStagePage.h"

#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompMask.h>

namespace eone
{
namespace mask
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node)
	: eone::WxStagePage(parent, node)
{
	m_sub_mgr.RegisterObserver(ee0::MSG_INSERT_SCENE_NODE, this);
	m_sub_mgr.RegisterObserver(ee0::MSG_DELETE_SCENE_NODE, this);
	m_sub_mgr.RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);

	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(library, this));
	}
}

void WxStagePage::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

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
	}
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
	                       const ee0::VariantSet& variants) const
{
	auto var = variants.GetVariant("preview");
	if (var.m_type == ee0::VT_EMPTY) {
		auto& cmask = m_node->GetComponent<n2::CompMask>();
		cmask.Traverse(func);
	} else {
		func(m_node);
	}
}

const n0::NodeComponent& WxStagePage::GetEditedNodeComp() const
{
	return m_node->GetComponent<n2::CompMask>();
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto& cmask = m_node->GetComponent<n2::CompMask>();
	if (cmask.GetBaseNode() && cmask.GetMaskNode()) {
		return;
	}

	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	if (!cmask.GetBaseNode()) {
		cmask.SetBaseNode(*node);
	} else {
		GD_ASSERT(!cmask.GetMaskNode(), "mask not null");
		cmask.SetMaskNode(*node);
	}
		
	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto& cmask = m_node->GetComponent<n2::CompMask>();
	if (cmask.GetBaseNode() == *node) {
		cmask.SetBaseNode(nullptr);
	} else {
		GD_ASSERT(cmask.GetMaskNode() == *node, "err mask");
		cmask.SetMaskNode(nullptr);
	}

	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::ClearSceneNode()
{
	auto& cmask = m_node->GetComponent<n2::CompMask>();
	cmask.SetBaseNode(nullptr);
	cmask.SetMaskNode(nullptr);

	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}
}