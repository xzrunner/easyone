#include "quake/EditorMgr.h"
#include "quake/WxStagePage.h"

#include <ee0/WxStagePage.h>

#include <ee3/CameraMgr.h>

#include <ee3/NodeRotateOP.h>
#include <ee3/NodeTranslateOP.h>
#include <ee3/NodeSelectOP.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/CameraDriveOP.h>
#include <ee3/CameraFlyOP.h>
#include <ee3/VertexSelectOP.h>
#include <ee3/VertexTranslateOP.h>
#include <ee3/EdgeSelectOP.h>
#include <ee3/EdgeTranslateOP.h>
#include <ee3/FaceSelectOP.h>
#include <ee3/FaceTranslateOP.h>
#include <ee3/PolySelectOP.h>
#include <ee3/PolyArrangeOP.h>

#if defined(__WXGTK__)
   #define HIDE_CURSOR wxSetCursor(wxCURSOR_BLANK)
   #define SHOW_CURSOR wxSetCursor(*wxSTANDARD_CURSOR)
#elif defined(__WXMSW__)
   #define HIDE_CURSOR ShowCursor(0)
   #define SHOW_CURSOR ShowCursor(1)
#endif

namespace eone
{
namespace quake
{

EditorMgr::EditorMgr(WxStagePage& m_stage)
	: m_stage(m_stage)
{
}

void EditorMgr::Init(const ee3::CameraMgr& cam_mgr,
	                 const std::shared_ptr<pt0::Camera>& cam,
	                 const pt3::Viewport& vp)
{
	auto sub_mgr = m_stage.GetSubjectMgr();

	auto& cam3d = cam_mgr.GetCamera(ee3::CameraMgr::CAM_3D);
	m_cam_drive_op = std::make_shared<ee3::CameraDriveOP>(cam3d, vp, sub_mgr);
	m_cam_fly_op   = std::make_shared<ee3::CameraFlyOP>(cam3d, sub_mgr);
	m_camera_op = m_cam_drive_op;

	auto select_op = std::make_shared<ee3::mesh::PolySelectOP>(cam, m_stage, vp);
	m_select_op = select_op;
	auto& selected = select_op->GetSelected();
	std::function<void()> update_cb = [select_op]() {
		select_op->UpdateCachedPolyBorder();
	};

	// arrange op with select, default
	m_default_op = std::make_shared<ee3::mesh::PolyArrangeOP>(cam, vp, sub_mgr, selected, update_cb);
	m_default_op->SetPrevEditOP(m_select_op);
	// rotate
	m_rotate_op = std::make_shared<ee3::NodeRotateOP>(cam, m_stage, vp);
	// translate
	m_translate_op = std::make_shared<ee3::NodeTranslateOP>(cam, m_stage, vp);
	// vertex
	auto select_vert_op = std::make_shared<ee3::mesh::VertexSelectOP>(cam, vp, sub_mgr, selected);
	select_vert_op->SetPrevEditOP(m_select_op);
	m_vertex_op = std::make_shared<ee3::mesh::VertexTranslateOP>(
		cam, vp, sub_mgr, selected, select_vert_op->GetSelected(), update_cb);
	m_vertex_op->SetPrevEditOP(select_vert_op);
	// edge
	auto select_edge_op = std::make_shared<ee3::mesh::EdgeSelectOP>(cam, vp, sub_mgr, selected);
	select_edge_op->SetPrevEditOP(m_select_op);
	m_edge_op = std::make_shared<ee3::mesh::EdgeTranslateOP>(
		cam, vp, sub_mgr, selected, select_edge_op->GetSelected(), update_cb);
	m_edge_op->SetPrevEditOP(select_edge_op);
	// face
	auto select_face_op = std::make_shared<ee3::mesh::FaceSelectOP>(cam, vp, sub_mgr, selected);
	select_face_op->SetPrevEditOP(m_select_op);
	m_face_op = std::make_shared<ee3::mesh::FaceTranslateOP>(
		cam, vp, sub_mgr, selected, select_face_op->GetSelected(), update_cb);
	m_face_op->SetPrevEditOP(select_face_op);

	ChangedCamOP(m_camera_op);

	auto& impl = m_stage.GetImpl();
	impl.SetEditOP(m_default_op);
	impl.SetOnKeyDownFunc([&, sub_mgr](int key_code)
	{
		auto select_op = std::dynamic_pointer_cast<ee3::mesh::PolySelectOP>(m_select_op);
		switch (key_code)
		{
		case 'R':
			if (!m_stage.GetSelection().IsEmpty()) {
				select_op->SetCanSelectNull(false);
				impl.SetEditOP(m_rotate_op);
				sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			}
			break;
		case 'T':
			if (!m_stage.GetSelection().IsEmpty()) {
				select_op->SetCanSelectNull(false);
				impl.SetEditOP(m_translate_op);
				sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			}
			break;

		case 'V':
			select_op->SetCanSelectNull(false);
			impl.SetEditOP(m_vertex_op);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case 'E':
			select_op->SetCanSelectNull(false);
			impl.SetEditOP(m_edge_op);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case 'F':
			select_op->SetCanSelectNull(false);
			impl.SetEditOP(m_face_op);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;

		case 'Y':
			if (m_camera_op == m_cam_drive_op) {
				m_camera_op = m_cam_fly_op;
				HIDE_CURSOR;
			} else {
				m_camera_op = m_cam_drive_op;
				SHOW_CURSOR;
			}
			ChangedCamOP(m_camera_op);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;

		case WXK_ESCAPE:
			select_op->SetCanSelectNull(true);
			impl.SetEditOP(m_default_op);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case WXK_SPACE:
			m_stage.SwitchToNextViewport();
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		}
	});
}

void EditorMgr::SetCamera(const pt0::CameraPtr& cam)
{
	m_cam_drive_op->SetCamera(cam);
	m_cam_fly_op->SetCamera(cam);
	m_select_op->SetCamera(cam);

	m_default_op->SetCamera(cam);
	m_rotate_op->SetCamera(cam);
	m_translate_op->SetCamera(cam);

	m_vertex_op->SetCamera(cam);
	m_edge_op->SetCamera(cam);
	m_face_op->SetCamera(cam);
}

void EditorMgr::ChangedCamOP(const ee0::EditOPPtr& cam_op)
{
	m_select_op->SetPrevEditOP(cam_op);
	m_rotate_op->SetPrevEditOP(cam_op);
	m_translate_op->SetPrevEditOP(cam_op);
}

}
}