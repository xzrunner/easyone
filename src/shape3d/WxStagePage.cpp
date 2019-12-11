#include "shape3d/WxStagePage.h"

#ifdef MODULE_SHAPE3D

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/WxStageSubPanel.h"
#include "frame/AppStyle.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/WxStageDropTarget.h>
#include <ee3/CameraFlyOP.h>
#include <ee3/MessageID.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <painting3/PerspCam.h>
#include <painting3/OrthoCam.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <sx/ResFileHelper.h>

namespace eone
{
namespace shape3d
{

const std::string WxStagePage::PAGE_TYPE = "shape3d";

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE)
	, m_editor_mgr(*this)
	, m_cam_mgr(false)
{
	m_messages.push_back(ee0::MSG_SCENE_NODE_INSERT);
	m_messages.push_back(ee0::MSG_SCENE_NODE_DELETE);
	m_messages.push_back(ee0::MSG_SCENE_NODE_CLEAR);
	m_messages.push_back(ee3::MSG_SWITCH_TO_NEXT_VIEWPORT);

	if (library) {
		SetDropTarget(new ee3::WxStageDropTarget(ECS_WORLD_VAR library, this));
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
	case ee3::MSG_SWITCH_TO_NEXT_VIEWPORT:
		SwitchToNextViewport();
		GetImpl().GetCanvas()->SetDirty();
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
		m_obj->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
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
		m_obj->GetSharedComp<n0::CompComplex>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
	}
}

void WxStagePage::InitEditOP(const std::shared_ptr<pt0::Camera>& cam, const pt3::Viewport& vp)
{
	m_editor_mgr.Init(m_cam_mgr, cam, vp);
	m_editor_mgr.SetCurrOp(draft3::EditOpMgr::Operator::DRAW);
}

void WxStagePage::InitViewports()
{
	auto canvas = std::dynamic_pointer_cast<ee3::WxStageCanvas>(GetImpl().GetCanvas());
	assert(canvas);
	auto cam = canvas->GetCamera();
	assert(cam->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>());
	m_cam_mgr.SetCamera(cam, ee3::CameraMgr::CAM_3D);
}

void WxStagePage::OnPageInit()
{
    auto stage_ext_panel = Blackboard::Instance()->GetStageExtPanel();
    stage_ext_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return nullptr;
    }, wxHORIZONTAL);
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n0::CompComplex>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::SwitchToNextViewport()
{
	auto& cam = m_cam_mgr.SwitchToNext();
	m_editor_mgr.SetCamera(cam);

	auto canvas = std::dynamic_pointer_cast<ee3::WxStageCanvas>(GetImpl().GetCanvas());

	auto& vp = canvas->GetViewport();
	cam->OnSize(vp.Width(), vp.Height());

	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
	if (wc) {
		wc->SetProjection(cam->GetProjectionMat());
	}

	canvas->SetCamera(cam);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	ccomplex.AddChild(*obj);
	m_editor_mgr.SetSelectedNode(*obj);
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj* obj = static_cast<const ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	if (ccomplex.RemoveChild(*obj)) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
#endif // GAME_OBJ_ECS
}

void WxStagePage::ClearSceneNode()
{
	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	ccomplex.RemoveAllChildren();
	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
#endif // GAME_OBJ_ECS
}

}
}

#endif // MODULE_SHAPE3D