#include "physics3d/PBDSceneOP.h"

#ifdef MODULE_PHYSICS3D

#include <ee2/CamZoomState.h>

#include <pbd/simulation.h>

namespace eone
{
namespace physics3d
{

PBDSceneOP::PBDSceneOP(const std::shared_ptr<pt0::Camera>& camera,
                       const ee0::SubjectMgrPtr& sub_mgr)
    : ee0::EditOP(camera)
{
    m_sim = std::make_unique<pbd::Simulation>();
    m_sim->Resize(glm::ivec2(10, 10));

    m_cam_op = std::make_unique<ee2::CamZoomState>(camera, sub_mgr);
}

bool PBDSceneOP::OnMouseWheelRotation(int x, int y, int direction)
{
    if (ee0::EditOP::OnMouseWheelRotation(x, y, direction)) {
        return true;
    }

    m_cam_op->OnMouseWheelRotation(x, y, direction);

    return false;
}

bool PBDSceneOP::OnDraw() const
{
    if (ee0::EditOP::OnDraw()) {
        return true;
    }

    m_sim->Draw();

    return false;
}

bool PBDSceneOP::Update(float dt)
{
    if (ee0::EditOP::Update(dt)) {
        return true;
    }

    m_sim->Tick(dt);

    return false;
}

}
}

#endif // MODULE_PHYSICS3D