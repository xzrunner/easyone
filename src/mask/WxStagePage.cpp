#include "mask/WxStagePage.h"

#ifdef MODULE_MASK

#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/AppStyle.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#else
#include <entity2/CompMask.h>
#endif // GAME_OBJ_ECS

namespace eone
{
namespace mask
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_LIBRARY | SHOW_RECORD | SHOW_STAGE | SHOW_PREVIEW | SHOW_WORLD | SHOW_DETAIL | SHOW_SCRIPT)
{
	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);

	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(ECS_WORLD_VAR library, this));
	}
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_SCENE_NODE_INSERT:
		InsertSceneNode(variants);
		break;
	case ee0::MSG_SCENE_NODE_DELETE:
		DeleteSceneNode(variants);
		break;
	case ee0::MSG_SCENE_NODE_CLEAR:
		ClearSceneNode();
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
		m_obj->GetSharedComp<n2::CompMask>().Traverse(func, inverse);
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
		m_obj->GetSharedComp<n2::CompMask>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
	}
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n2::CompMask>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
#ifndef GAME_OBJ_ECS
	auto& cmask = m_obj->GetSharedComp<n2::CompMask>();
	if (cmask.GetBaseNode() && cmask.GetMaskNode()) {
		return;
	}
#else
	auto& cmask = m_world.GetComponent<e2::CompMask>(m_obj);
	if (!cmask.base.IsNull() && !cmask.mask.IsNull()) {
		return;
	}
#endif // GAME_OBJ_ECS

	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

#ifndef GAME_OBJ_ECS
	if (!cmask.GetBaseNode()) {
		cmask.SetBaseNode(*obj);
	} else {
		GD_ASSERT(!cmask.GetMaskNode(), "mask not null");
		cmask.SetMaskNode(*obj);
	}
#else
	if (cmask.base.IsNull()) {
		cmask.base = *obj;
	} else {
		GD_ASSERT(cmask.mask.IsNull(), "mask not null");
		cmask.mask = *obj;
	}
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

#ifndef GAME_OBJ_ECS
	auto& cmask = m_obj->GetSharedComp<n2::CompMask>();
	if (cmask.GetBaseNode() == *obj) {
		cmask.SetBaseNode(nullptr);
	} else {
		GD_ASSERT(cmask.GetMaskNode() == *obj, "err mask");
		cmask.SetMaskNode(nullptr);
	}
#else
	auto& cmask = m_world.GetComponent<e2::CompMask>(m_obj);
	if (cmask.base == *obj) {
		m_world.DestroyEntity(cmask.base);
		cmask.base.id = 0;
	} else {
		GD_ASSERT(cmask.mask == *obj, "err mask");
		cmask.mask.id = 0;
	}
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::ClearSceneNode()
{
#ifndef GAME_OBJ_ECS
	auto& cmask = m_obj->GetSharedComp<n2::CompMask>();
	cmask.SetBaseNode(nullptr);
	cmask.SetMaskNode(nullptr);
#else
	auto& cmask = m_world.GetComponent<e2::CompMask>(m_obj);
	m_world.DestroyEntity(cmask.base);
	m_world.DestroyEntity(cmask.mask);
	cmask.base.id = 0;
	cmask.mask.id = 0;
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}
}

#endif // MODULE_MASK