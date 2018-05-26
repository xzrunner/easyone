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
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompTransform.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompScale9.h>
#include <ns/NodeSerializer.h>
#else
#include <entity2/SysTransform.h>
#include <entity2/CompScale9.h>
#include <entity2/CompBoundingBox.h>
#endif // GAME_OBJ_ECS

namespace eone
{
namespace scale9
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, LAYOUT_PREVIEW)
{
	m_messages.push_back(ee0::MSG_INSERT_SCENE_NODE);
	m_messages.push_back(ee0::MSG_DELETE_SCENE_NODE);
	m_messages.push_back(ee0::MSG_CLEAR_SCENE_NODE);

	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(ECS_WORLD_VAR library, this));
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

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
	                       const ee0::VariantSet& variants,
	                       bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY) {
		TraverseGrids(func);
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		func(m_obj);
		break;
	default:
		TraverseGrids(func);
	}
}

void WxStagePage::OnPageInit()
{
	auto preview = Blackboard::Instance()->GetPreviewPanel();
	auto op = std::make_shared<ResizeScale9OP>(preview, ECS_WORLD_SELF_VAR m_obj);
	preview->GetImpl().SetEditOP(op);
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n2::CompScale9>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val)
{
	auto& grids_val = val["grids"];
	for (auto itr = grids_val.Begin(); itr != grids_val.End(); ++itr)
	{
#ifndef GAME_OBJ_ECS
		auto obj = std::make_shared<n0::SceneNode>();
		ns::NodeSerializer::LoadFromJson(obj, dir, *itr);
#else
		auto obj = m_world.CreateEntity();
		// todo ecs load
#endif // GAME_OBJ_ECS

#ifndef GAME_OBJ_ECS
		auto& ctrans = obj->GetUniqueComp<n2::CompTransform>();
		auto& pos = ctrans.GetTrans().GetPosition();
#else
		auto& pos = e2::SysTransform::GetPosition(m_world, obj);
#endif // GAME_OBJ_ECS
		int col, row;
		ComposeGrids::Query(pos, &col, &row);
		if (col == -1 || row == -1) {
			continue;
		}

#ifndef GAME_OBJ_ECS
		ctrans.SetPosition(*obj, ComposeGrids::GetGridCenter(col, row));
#else
		e2::SysTransform::SetPosition(m_world, obj, ComposeGrids::GetGridCenter(col, row));
#endif // GAME_OBJ_ECS

		const int idx = row * 3 + col;
		if (GAME_OBJ_VALID(m_grids[idx])) {
			ee0::MsgHelper::DeleteNode(*m_sub_mgr, m_grids[idx]);
		}
		m_grids[idx] = obj;
	}

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	int col, row;
#ifndef GAME_OBJ_ECS
	auto& ctrans = (*obj)->GetUniqueComp<n2::CompTransform>();
	auto& pos = ctrans.GetTrans().GetPosition();
#else
	auto pos = e2::SysTransform::GetPosition(m_world, *obj);
#endif // GAME_OBJ_ECS
	ComposeGrids::Query(pos, &col, &row);
	if (col == -1 || row == -1) {
		return;
	}

#ifndef GAME_OBJ_ECS
	ctrans.SetPosition(**obj, ComposeGrids::GetGridCenter(col, row));
#else
	e2::SysTransform::SetPosition(m_world, *obj, ComposeGrids::GetGridCenter(col, row));
#endif // GAME_OBJ_ECS

	const int idx = row * 3 + col;
	if (GAME_OBJ_VALID(m_grids[idx])) {
		ee0::MsgHelper::DeleteNode(*m_sub_mgr, m_grids[idx]);
	}
	m_grids[idx] = *obj;

#ifndef GAME_OBJ_ECS
	auto& cscale9 = m_obj->GetSharedComp<n2::CompScale9>();
	auto type = n2::CompScale9::CheckType(m_grids);
	cscale9.Build(type, cscale9.GetWidth(), cscale9.GetHeight(), m_grids, 0, 0, 0, 0);
#else
	auto& cscale9 = m_world.GetComponent<e2::CompScale9>(m_obj);
	auto type = cscale9.type;
	// todo ecs
#endif // GAME_OBJ_ECS

	// update bounding
#ifndef GAME_OBJ_ECS
	if (type != n2::CompScale9::S9_NULL) 
	{
		auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
		cbb.SetSize(*m_obj, sm::rect(cscale9.GetWidth(), cscale9.GetHeight()));
	}
#else
	if (type != e2::CompScale9::S9_NULL)
	{
		auto& cbb = m_world.GetComponent<e2::CompBoundingBox>(m_obj);
		cbb.rect = sm::rect(cscale9.width, cscale9.height);
	}
#endif // GAME_OBJ_ECS
	
	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	for (auto& grid : m_grids) 
	{
		if (grid == *obj)
		{
#ifndef GAME_OBJ_ECS
			grid.reset();
#else
			grid.Reset();
			m_world.DestroyEntity(grid);
#endif // GAME_OBJ_ECS
		}
	}

#ifndef GAME_OBJ_ECS
	auto& cscale9 = m_obj->GetSharedComp<n2::CompScale9>();
	auto type = n2::CompScale9::CheckType(m_grids);
	cscale9.Build(type, cscale9.GetWidth(), cscale9.GetHeight(), m_grids, 0, 0, 0, 0);
#else
	// todo ecs
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::ClearSceneNode()
{
	for (auto& grid : m_grids) 
	{
#ifndef GAME_OBJ_ECS
		grid.reset();
#else
		grid.Reset();
		m_world.DestroyEntity(grid);
#endif // GAME_OBJ_ECS	
	}

#ifndef GAME_OBJ_ECS
	auto& cscale9 = m_obj->GetSharedComp<n2::CompScale9>();
	cscale9.Build(n2::CompScale9::S9_NULL, cscale9.GetWidth(), cscale9.GetHeight(), m_grids, 0, 0, 0, 0);
#else
	// todo ecs
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::TraverseGrids(std::function<bool(const ee0::GameObj&)> func) const
{
	for (int i = 0; i < 9; ++i) {
		if (GAME_OBJ_VALID(m_grids[i])) {
			func(m_grids[i]);
		}
	}
}

}
}