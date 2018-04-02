#include "particle3d/PlayParticlesOP.h"

#include <ee0/CameraHelper.h>

#include <emitter/P3dInstance.h>
#include <node0/SceneNode.h>
#include <node2/CompParticle3dInst.h>
#include <node2/CompTransform.h>

namespace eone
{
namespace particle3d
{

PlayParticlesOP::PlayParticlesOP(const n0::SceneNodePtr& node, pt2::Camera& cam,
	                             const ee0::SubjectMgrPtr& sub_mgr)
	: ee2::CamControlOP(cam, sub_mgr)
	, m_node(node)
{
}

bool PlayParticlesOP::OnMouseLeftDown(int x, int y)
{
	if (ee2::CamControlOP::OnMouseLeftDown(x, y)) {
		return true;
	}

	auto pos = ee0::CameraHelper::TransPosScreenToProject(m_cam, x, y);

	auto& ctrans = m_node->GetUniqueComp<n2::CompTransform>();
	ctrans.SetPosition(*m_node, pos);

	auto& p3d_inst = m_node->GetUniqueComp<n2::CompParticle3dInst>().GetP3dInst();
	p3d_inst.SetPos(pos.x, pos.y);
	p3d_inst.Start();

	return false;
}

bool PlayParticlesOP::OnMouseDrag(int x, int y)
{
	if (ee2::CamControlOP::OnMouseDrag(x, y)) {
		return true;
	}

	auto pos = ee0::CameraHelper::TransPosScreenToProject(m_cam, x, y);

	auto& ctrans = m_node->GetUniqueComp<n2::CompTransform>();
	ctrans.SetPosition(*m_node, pos);

	auto& p3d_inst = m_node->GetUniqueComp<n2::CompParticle3dInst>().GetP3dInst();
	p3d_inst.SetPos(pos.x, pos.y);

	return false;
}

}
}