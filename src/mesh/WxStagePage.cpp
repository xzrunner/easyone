#include "mesh/WxStagePage.h"

#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/AppStyle.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompMesh.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompShape.h>
#else
#include <entity2/CompMesh.h>
#endif // GAME_OBJ_ECS

namespace eone
{
namespace mesh
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_LIBRARY | SHOW_STAGE | SHOW_PREVIEW)
{
	m_messages.push_back(ee0::MSG_INSERT_SCENE_NODE);

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
	}
}

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
	                       const ee0::VariantSet& variants,
	                       bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY)
	{
		// todo ecs
#ifndef GAME_OBJ_ECS
		m_obj->GetSharedComp<n2::CompMesh>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		func(m_obj);
		break;
		// todo ecs
#ifndef GAME_OBJ_ECS
	default:
		m_obj->GetSharedComp<n2::CompMesh>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
	}
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n2::CompMesh>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	GD_ASSERT((*obj)->HasUniqueComp<n2::CompBoundingBox>(), "no boundingbox");
	auto& sz = (*obj)->GetUniqueComp<n2::CompBoundingBox>().GetSize();

	auto& cmesh = m_obj->GetSharedComp<n2::CompMesh>();
	cmesh.SetMesh(std::make_unique<pt2::Mesh<n0::SceneNodePtr>>(*obj, sz.Width(), sz.Height()));
	if (cmesh.GetMesh()->GetBaseSymbol()) {
		return;
	}

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}
}