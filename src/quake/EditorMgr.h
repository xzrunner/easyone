#pragma once

#include <ee0/typedef.h>

#include <painting0/Camera.h>

namespace ee3 { class CameraMgr; }
namespace pt0 { class Camera; }
namespace pt3 { class Viewport; }

namespace eone
{
namespace quake
{

class WxStagePage;

class EditorMgr
{
public:
	EditorMgr(WxStagePage& stage);

	void Init(const ee3::CameraMgr& cam_mgr,
		const std::shared_ptr<pt0::Camera>& cam,
		const pt3::Viewport& vp);

	void SetCamera(const pt0::CameraPtr& cam);

private:
	void ChangedCamOP(const ee0::EditOPPtr& cam_op);

private:
	WxStagePage& m_stage;

	ee0::EditOPPtr m_cam_drive_op = nullptr;
	ee0::EditOPPtr m_cam_fly_op   = nullptr;
	ee0::EditOPPtr m_camera_op    = nullptr;
	ee0::EditOPPtr m_select_op    = nullptr;

	ee0::EditOPPtr m_default_op   = nullptr;
	ee0::EditOPPtr m_rotate_op    = nullptr;
	ee0::EditOPPtr m_translate_op = nullptr;

	ee0::EditOPPtr m_vertex_op    = nullptr;
	ee0::EditOPPtr m_edge_op      = nullptr;
	ee0::EditOPPtr m_face_op      = nullptr;

}; // EditorMgr

}
}