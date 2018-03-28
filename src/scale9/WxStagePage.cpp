#include "scale9/WxStagePage.h"
#include "scale9/ComposeGrids.h"
#include "scale9/ResizeScale9OP.h"

#include "frame/Blackboard.h"
#include "frame/WxPreviewPanel.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"

#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompScale9.h>
#include <ns/NodeSerializer.h>

namespace eone
{
namespace scale9
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node)
	: eone::WxStagePage(parent, node, SUB_WND_PREVIEW)
{
	m_messages.push_back(ee0::MSG_INSERT_SCENE_NODE);
	m_messages.push_back(ee0::MSG_DELETE_SCENE_NODE);
	m_messages.push_back(ee0::MSG_CLEAR_SCENE_NODE);

	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(library, this));
	}
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
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
	                       const ee0::VariantSet& variants,
	                       bool inverse) const
{
	auto var = variants.GetVariant("preview");
	if (var.m_type == ee0::VT_EMPTY) 
	{
		for (int i = 0; i < 9; ++i) {
			if (m_grids[i]) {
				func(m_grids[i]);
			}
		}
	} 
	else 
	{
		func(m_node);
	}
}

void WxStagePage::OnPageInit()
{
	auto preview = Blackboard::Instance()->GetPreviewPanel();
	auto op = std::make_shared<ResizeScale9OP>(preview, m_node);
	preview->GetImpl().SetEditOP(op);
}

const n0::NodeSharedComp& WxStagePage::GetEditedNodeComp() const
{
	return m_node->GetSharedComp<n2::CompScale9>();
}

void WxStagePage::LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val)
{
	auto& grids_val = val["grids"];
	for (auto itr = grids_val.Begin(); itr != grids_val.End(); ++itr)
	{
		auto node = std::make_shared<n0::SceneNode>();
		ns::NodeSerializer::LoadNodeFromJson(node, dir, *itr);

		auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
		int col, row;
		ComposeGrids::Query(ctrans.GetTrans().GetPosition(), &col, &row);
		if (col == -1 || row == -1) {
			continue;
		}

		ctrans.SetPosition(*node, ComposeGrids::GetGridCenter(col, row));

		const int idx = row * 3 + col;
		if (m_grids[idx]) {
			ee0::MsgHelper::DeleteNode(*m_sub_mgr, m_grids[idx]);
		}
		m_grids[idx] = node;
	}

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	auto& ctrans = (*node)->GetUniqueComp<n2::CompTransform>();
	int col, row;
	ComposeGrids::Query(ctrans.GetTrans().GetPosition(), &col, &row);
	if (col == -1 || row == -1) {
		return;
	}
	
	ctrans.SetPosition(**node, ComposeGrids::GetGridCenter(col, row));

	const int idx = row * 3 + col;
	if (m_grids[idx]) {
		ee0::MsgHelper::DeleteNode(*m_sub_mgr, m_grids[idx]);
	}
	m_grids[idx] = *node;

	auto& cscale9 = m_node->GetSharedComp<n2::CompScale9>();
	auto type = n2::CompScale9::CheckType(m_grids);
	cscale9.Build(type, cscale9.GetWidth(), cscale9.GetHeight(), m_grids, 0, 0, 0, 0);

	// update bounding
	if (type != n2::CompScale9::S9_NULL) 
	{
		auto& cbb = m_node->GetUniqueComp<n2::CompBoundingBox>();
		cbb.SetSize(*m_node, sm::rect(cscale9.GetWidth(), cscale9.GetHeight()));
	}
	
	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("node");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: node");
	n0::SceneNodePtr* node = static_cast<n0::SceneNodePtr*>(var.m_val.pv);
	GD_ASSERT(node, "err scene node");

	for (int i = 0; i < 9; ++i) {
		if (m_grids[i] == *node) {
			m_grids[i].reset();
		}
	}

	auto& cscale9 = m_node->GetSharedComp<n2::CompScale9>();
	auto type = n2::CompScale9::CheckType(m_grids);
	cscale9.Build(type, cscale9.GetWidth(), cscale9.GetHeight(), m_grids, 0, 0, 0, 0);

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::ClearSceneNode()
{
	for (int i = 0; i < 9; ++i) {
		m_grids[i].reset();
	}

	auto& cscale9 = m_node->GetSharedComp<n2::CompScale9>();
	cscale9.Build(n2::CompScale9::S9_NULL, cscale9.GetWidth(), cscale9.GetHeight(), m_grids, 0, 0, 0, 0);

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}
}