#pragma once

#include "frame/config.h"

#ifdef MODULE_PHYSICS3D

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

namespace pbd { class Simulation; }
namespace ee0 { class EditOpState; }

namespace eone
{
namespace physics3d
{

class PBDSceneOP : public ee0::EditOP
{
public:
    PBDSceneOP(const std::shared_ptr<pt0::Camera>& camera,
        const ee0::SubjectMgrPtr& sub_mgr);

    virtual bool OnMouseWheelRotation(int x, int y, int direction) override;

    virtual bool OnDraw() const;
    virtual bool Update(float dt);

private:
    std::unique_ptr<pbd::Simulation> m_sim = nullptr;

    std::unique_ptr<ee0::EditOpState> m_cam_op = nullptr;

}; // PBDSceneOP

}
}

#endif // MODULE_PHYSICS3D