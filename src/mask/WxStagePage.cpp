#include "mask/WxStagePage.h"

#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <node2/CompMask.h>

namespace eone
{
namespace mask
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library,
	                     n2::CompMask& cmask)
	: ee0::WxStagePage(parent)
	, m_cmask(cmask)
{
	m_sub_mgr.RegisterObserver(ee0::MSG_INSERT_SCENE_NODE, this);
	m_sub_mgr.RegisterObserver(ee0::MSG_DELETE_SCENE_NODE, this);
	m_sub_mgr.RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);

	SetDropTarget(new ee2::WxStageDropTarget(library, this));
}

void WxStagePage::OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants)
{
	ee0::WxStagePage::OnNotify(msg, variants);

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

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const
{
	m_cmask.Traverse(func);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	if (m_cmask.GetBaseNode() && m_cmask.GetMaskNode()) {
		return;
	}

	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	if (!m_cmask.GetBaseNode()) {
		m_cmask.SetBaseNode(*node);
	} else {
		GD_ASSERT(!m_cmask.GetMaskNode(), "mask not null");
		m_cmask.SetMaskNode(*node);
	}
		
	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	if (m_cmask.GetBaseNode() == *node) {
		m_cmask.SetBaseNode(nullptr);
	} else {
		GD_ASSERT(m_cmask.GetMaskNode() == *node, "err mask");
		m_cmask.SetMaskNode(nullptr);
	}

	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::ClearSceneNode()
{
	m_cmask.SetBaseNode(nullptr);
	m_cmask.SetMaskNode(nullptr);

	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}
}