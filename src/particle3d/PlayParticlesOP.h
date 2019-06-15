#pragma once

#include "frame/config.h"

#ifdef MODULE_PARTICLE3D

#include <ee2/CamControlOP.h>

#include <node0/typedef.h>

namespace eone
{
namespace particle3d
{

class PlayParticlesOP : public ee2::CamControlOP
{
public:
	PlayParticlesOP(const std::shared_ptr<pt0::Camera>& camera,
		const n0::SceneNodePtr& node, const ee0::SubjectMgrPtr& sub_mgr);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

private:
	n0::SceneNodePtr m_node;

}; // PlayParticlesOP

}
}

#endif // MODULE_PARTICLE3D