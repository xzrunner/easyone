#include "scene3d/WxStagePage.h"

#include "frame/WxStagePage.h"

#include <ee3/WxStageDropTarget.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>

namespace eone
{
namespace scene3d
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node)
	: eone::WxStagePage(parent, node)
{
	m_sub_mgr.RegisterObserver(ee0::MSG_INSERT_SCENE_NODE, this);
	m_sub_mgr.RegisterObserver(ee0::MSG_DELETE_SCENE_NODE, this);
	m_sub_mgr.RegisterObserver(ee0::MSG_CLEAR_SCENE_NODE, this);

	if (library) {
		SetDropTarget(new ee3::WxStageDropTarget(library, this));
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
		auto& ccomplex = m_node->GetSharedComp<n0::CompComplex>();
		ccomplex.Traverse(func);
	} else {
		func(m_node);
	}
}

const n0::NodeSharedComp& WxStagePage::GetEditedNodeComp() const
{
	return m_node->GetSharedComp<n0::CompComplex>();
}

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val, 
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
	val.AddMember("camera", "3d", alloc);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto& ccomplex = m_node->GetSharedComp<n0::CompComplex>();
	if (m_node_selection.IsEmpty()) {
		ccomplex.AddChild(*node);
	}

	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto& ccomplex = m_node->GetSharedComp<n0::CompComplex>();
	if (ccomplex.RemoveChild(*node)) {
		m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
}

void WxStagePage::ClearSceneNode()
{
	auto& ccomplex = m_node->GetSharedComp<n0::CompComplex>();
	ccomplex.RemoveAllChildren();
	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}
}