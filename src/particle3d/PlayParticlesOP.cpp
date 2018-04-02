#include "particle3d/PlayParticlesOP.h"

#include <ee0/CameraHelper.h>

#include <emitter/P3dInstance.h>

namespace eone
{
namespace particle3d
{

PlayParticlesOP::PlayParticlesOP(et::P3dInstance& p3d_inst, pt2::Camera& cam, 
	                             const ee0::SubjectMgrPtr& sub_mgr)
	: ee2::CamControlOP(cam, sub_mgr)
	, m_p3d_inst(p3d_inst)
{
}

bool PlayParticlesOP::OnMouseLeftDown(int x, int y)
{
	if (ee2::CamControlOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	auto pos = ee0::CameraHelper::TransPosScreenToProject(m_cam, x, y);
	m_p3d_inst.SetPos(pos.x, pos.y);
	m_p3d_inst.Start();

	return false;
}

bool PlayParticlesOP::OnMouseDrag(int x, int y)
{
	if (ee2::CamControlOP::OnMouseDrag(x, y)) {
		return true;
	}

	return false;
}

}
}