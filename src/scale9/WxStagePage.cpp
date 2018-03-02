#include "scale9/WxStagePage.h"
#include "scale9/ComposeGrids.h"

#include <ee2/WxStageDropTarget.h>

#include <node0/SceneNode.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompScale9.h>
#include <guard/check.h>

namespace eone
{
namespace scale9
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node)
	: ee0::WxStagePage(parent)
	, m_node(node)
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

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
	                       const ee0::VariantSet& variants) const
{
	auto var = variants.GetVariant("preview");
	if (var.m_type == ee0::VT_EMPTY) {
		auto& cscale9 = m_node->GetComponent<n2::CompScale9>();
		cscale9.Traverse(func);
	} else {
		func(m_node);
	}
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto& ctrans = (*node)->GetComponent<n2::CompTransform>();
	int col, row;
	ComposeGrids::Query(ctrans.GetTrans().GetPosition(), &col, &row);
	if (col == -1 || row == -1) {
		return;
	}

	ctrans.GetTrans().SetPosition(ComposeGrids::GetGridCenter(col, row));
	(*node)->GetComponent<n2::CompBoundingBox>().Build(ctrans.GetTrans().GetSRT());

	auto& cscale9 = m_node->GetComponent<n2::CompScale9>();
	cscale9.SetNode(row * 3 + col, *node);
	
	m_sub_mgr.NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{

}

void WxStagePage::ClearSceneNode()
{

}

bool WxStagePage::DeleteSceneNode(const n0::SceneNodePtr& node)
{
	return false;
}

}
}