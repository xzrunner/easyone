#include "quake/WxStagePage.h"
#include "quake/QuakeMapLoader.h"
#include "quake/WxPreviewPanel.h"
#include "quake/WxPreviewCanvas.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/WxStageExtPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxStageDropTarget.h>
#include <ee3/NodeRotateOP.h>
#include <ee3/NodeTranslateOP.h>
#include <ee3/NodeSelectOP.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/CameraDriveOP.h>
#include <ee3/VertexSelectOP.h>
#include <ee3/VertexTranslateOP.h>
#include <ee3/EdgeSelectOP.h>
#include <ee3/EdgeTranslateOP.h>
#include <ee3/FaceSelectOP.h>
#include <ee3/FaceTranslateOP.h>
#include <ee3/PolySelectOP.h>
#include <ee3/PolyArrangeOP.h>

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

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, LAYOUT_STAGE_EXT)
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
	auto& impl = GetImpl();

	m_camera_op = std::make_shared<ee3::CameraDriveOP>(m_cam_mgr.GetCamera(ee3::CameraMgr::CAM_3D), vp, m_sub_mgr);
	auto select_op = std::make_shared<ee3::mesh::PolySelectOP>(cam, *this, vp);
	m_select_op = select_op;
	auto& selected = select_op->GetSelected();
	m_select_op->SetPrevEditOP(m_camera_op);
	std::function<void()> update_cb = [select_op]() {
		select_op->UpdateCachedPolyBorder();
	};
	// arrange op with select, default
	m_default_op = std::make_shared<ee3::mesh::PolyArrangeOP>(cam, vp, m_sub_mgr, selected, update_cb);
	m_default_op->SetPrevEditOP(m_select_op);
	// rotate
	m_rotate_op = std::make_shared<ee3::NodeRotateOP>(cam, *this, vp);
	m_rotate_op->SetPrevEditOP(m_camera_op);
	// translate
	m_translate_op = std::make_shared<ee3::NodeTranslateOP>(cam, *this, vp);
	m_translate_op->SetPrevEditOP(m_camera_op);
	// vertex
	auto select_vert_op = std::make_shared<ee3::mesh::VertexSelectOP>(cam, vp, m_sub_mgr, selected);
	select_vert_op->SetPrevEditOP(m_select_op);
	m_vertex_op = std::make_shared<ee3::mesh::VertexTranslateOP>(
		cam, vp, m_sub_mgr, selected, select_vert_op->GetSelected(), update_cb);
	m_vertex_op->SetPrevEditOP(select_vert_op);
	// edge
	auto select_edge_op = std::make_shared<ee3::mesh::EdgeSelectOP>(cam, vp, m_sub_mgr, selected);
	select_edge_op->SetPrevEditOP(m_select_op);
	m_edge_op = std::make_shared<ee3::mesh::EdgeTranslateOP>(
		cam, vp, m_sub_mgr, selected, select_edge_op->GetSelected(), update_cb);
	m_edge_op->SetPrevEditOP(select_edge_op);
	// face
	auto select_face_op = std::make_shared<ee3::mesh::FaceSelectOP>(cam, vp, m_sub_mgr, selected);
	select_face_op->SetPrevEditOP(m_select_op);
	m_face_op = std::make_shared<ee3::mesh::FaceTranslateOP>(
		cam, vp, m_sub_mgr, selected, select_face_op->GetSelected(), update_cb);
	m_face_op->SetPrevEditOP(select_face_op);

	impl.SetEditOP(m_default_op);

	GetImpl().SetOnKeyDownFunc([&](int key_code)
	{
		auto select_op = std::dynamic_pointer_cast<ee3::mesh::PolySelectOP>(m_select_op);
		switch (key_code)
		{
		case 'R':
			if (!GetSelection().IsEmpty()) {
				select_op->SetCanSelectNull(false);
				impl.SetEditOP(m_rotate_op);
				m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			}
			break;
		case 'T':
			if (!GetSelection().IsEmpty()) {
				select_op->SetCanSelectNull(false);
				impl.SetEditOP(m_translate_op);
				m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			}
			break;

		case 'V':
			select_op->SetCanSelectNull(false);
			impl.SetEditOP(m_vertex_op);
			m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case 'E':
			select_op->SetCanSelectNull(false);
			impl.SetEditOP(m_edge_op);
			m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case 'F':
			select_op->SetCanSelectNull(false);
			impl.SetEditOP(m_face_op);
			m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;

		case WXK_ESCAPE:
			select_op->SetCanSelectNull(true);
			impl.SetEditOP(m_default_op);
			m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case WXK_SPACE:
			SwitchToNextViewport();
			m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		}
	});
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

	auto preview_op = std::make_shared<ee3::CameraDriveOP>(
		preview_canvas->GetCamera(), preview_canvas->GetViewport(), preview_panel->GetSubjectMgr());
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
	val.AddMember("is_scene3d", true, alloc);
}

void WxStagePage::LoadFromFileImpl(const std::string& filepath)
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

void WxStagePage::SwitchToNextViewport()
{
	auto& cam = m_cam_mgr.SwitchToNext();

	{
		m_camera_op->SetCamera(cam);
		m_select_op->SetCamera(cam);

		m_default_op->SetCamera(cam);
		m_rotate_op->SetCamera(cam);
		m_translate_op->SetCamera(cam);

		m_vertex_op->SetCamera(cam);
		m_edge_op->SetCamera(cam);
		m_face_op->SetCamera(cam);
	}

	auto canvas = std::dynamic_pointer_cast<ee3::WxStageCanvas>(GetImpl().GetCanvas());

	auto& vp = canvas->GetViewport();
	cam->OnSize(vp.Width(), vp.Height());

	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
	if (wc) {
		wc->SetProjection(cam->GetProjectionMat());
	}

	canvas->SetCamera(cam);
}

}
}