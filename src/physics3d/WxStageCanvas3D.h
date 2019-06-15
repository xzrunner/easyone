#pragma once

#include "frame/config.h"

#ifdef MODULE_PHYSICS3D

#include "frame/WxStageCanvas3D.h"

#define PHYSICS_SCENE_PBD

#ifdef PHYSICS_SCENE_PBD
#include <pbd/simulation.h>
#endif // PHYSICS_SCENE_PBD

namespace eone
{
namespace physics3d
{

class WxStageCanvas3D : public eone::WxStageCanvas3D
{
public:
	WxStageCanvas3D(eone::WxStagePage* stage, const ee0::RenderContext& rc);

protected:
	virtual bool OnUpdate() override;

private:
    clock_t m_last_time = 0;

#ifdef PHYSICS_SCENE_PBD
    pbd::Simulation m_sim;
#endif // PHYSICS_SCENE_PBD

}; // WxStageCanvas3D

}
}

#endif // MODULE_PHYSICS3D