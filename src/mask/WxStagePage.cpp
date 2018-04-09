#include "mask/WxStagePage.h"

#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node2/CompMask.h>

namespace eone
{
namespace mask
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const ee0::GameObj& obj)
	: eone::WxStagePage(parent, obj, LAYOUT_PREVIEW)
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

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
	                       const ee0::VariantSet& variants,
	                       bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY) {
		m_obj->GetSharedComp<n2::CompMask>().Traverse(func, inverse);
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		func(m_obj);
		break;
	default:
		m_obj->GetSharedComp<n2::CompMask>().Traverse(func, inverse);
	}
}

const n0::NodeSharedComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n2::CompMask>();
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto& cmask = m_obj->GetSharedComp<n2::CompMask>();
	if (cmask.GetBaseNode() && cmask.GetMaskNode()) {
		return;
	}

	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	if (!cmask.GetBaseNode()) {
		cmask.SetBaseNode(*obj);
	} else {
		GD_ASSERT(!cmask.GetMaskNode(), "mask not null");
		cmask.SetMaskNode(*obj);
	}
		
	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	auto& cmask = m_obj->GetSharedComp<n2::CompMask>();
	if (cmask.GetBaseNode() == *obj) {
		cmask.SetBaseNode(nullptr);
	} else {
		GD_ASSERT(cmask.GetMaskNode() == *obj, "err mask");
		cmask.SetMaskNode(nullptr);
	}

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::ClearSceneNode()
{
	auto& cmask = m_obj->GetSharedComp<n2::CompMask>();
	cmask.SetBaseNode(nullptr);
	cmask.SetMaskNode(nullptr);

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

}
}