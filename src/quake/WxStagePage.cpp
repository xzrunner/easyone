#include "quake/WxStagePage.h"
#include "quake/QuakeMapLoader.h"
#include "quake/WxPreviewPanel.h"
#include "quake/WxPreviewCanvas.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/WxStageExtPanel.h"
#include "frame/AppStyle.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxStageDropTarget.h>
#include <ee3/CameraFlyOP.h>

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
namespace quake
{

const std::string WxStagePage::PAGE_TYPE = "quake";

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_LIBRARY | SHOW_RECORD | SHOW_STAGE | SHOW_STAGE_EXT | SHOW_WORLD | SHOW_DETAIL | SHOW_SCRIPT)
	, m_editor_mgr(*this)
	, m_cam_mgr(false)
{
	m_messages.push_back(ee0::MSG_INSERT_SCENE_NODE);
	m_messages.push_back(ee0::MSG_DELETE_SCENE_NODE);
	m_messages.push_back(ee0::MSG_CLEAR_SCENE_NODE);

	if (library) {
		SetDropTarget(new ee3::WxStageDropTarget(ECS_WORLD_VAR library, this));
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
}

void WxStagePage::InitViewports()
{
	auto canvas = std::dynamic_pointer_cast<ee3::WxStageCanvas>(GetImpl().GetCanvas());
	assert(canvas);
	auto cam = canvas->GetCamera();
	assert(cam->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>());
	m_cam_mgr.SetCamera(cam, ee3::CameraMgr::CAM_3D);
}

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

void WxStagePage::OnPageInit()
{
	auto bb = Blackboard::Instance();

	auto panel = bb->GetStageExtPanel();
	auto sizer = panel->GetSizer();
	if (sizer) {
		sizer->Clear(true);
	} else {
		sizer = new wxBoxSizer(wxHORIZONTAL);
	}

	auto preview_panel = new WxPreviewPanel(panel);
	sizer->Add(preview_panel, 1, wxEXPAND);

	auto preview_canvas = std::make_shared<WxPreviewCanvas>(
		preview_panel, bb->GetRenderContext());
	preview_panel->GetImpl().SetCanvas(preview_canvas);

	auto preview_op = std::make_shared<ee3::CameraFlyOP>(
		preview_canvas->GetCamera(), preview_panel->GetSubjectMgr());
	preview_panel->GetImpl().SetEditOP(preview_op);

	panel->SetSizer(sizer);
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n0::CompComplex>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
	val.AddMember("page_type", rapidjson::Value(PAGE_TYPE.c_str(), alloc), alloc);
}

void WxStagePage::LoadFromFileExt(const std::string& filepath)
{
	if (sx::ResFileHelper::Type(filepath) != sx::RES_FILE_MAP) {
		return;
	}

	QuakeMapLoader::LoadFromFile(*m_sub_mgr, filepath);
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("obj");
	GD_ASSERT(var.m_type == ee0::VT_PVOID, "no var in vars: obj");
	ee0::GameObj* obj = static_cast<ee0::GameObj*>(var.m_val.pv);
	GD_ASSERT(obj, "err scene obj");

	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
	if (m_selection.IsEmpty()) {
		ccomplex.AddChild(*obj);
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