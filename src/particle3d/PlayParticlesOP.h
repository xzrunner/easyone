#pragma once

#include <ee2/CamControlOP.h>

namespace et { class P3dInstance; }

namespace eone
{
namespace particle3d
{

class PlayParticlesOP : public ee2::CamControlOP
{
public:
	PlayParticlesOP(et::P3dInstance& p3d_inst, pt2::Camera& cam, 
		const ee0::SubjectMgrPtr& sub_mgr);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

private:
	et::P3dInstance& m_p3d_inst;

}; // PlayParticlesOP

}
}