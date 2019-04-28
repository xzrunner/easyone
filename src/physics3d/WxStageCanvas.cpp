#include "physics3d/WxStageCanvas.h"
#include "physics3d/WxStagePage.h"

namespace eone
{
namespace physics3d
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, const ee0::RenderContext& rc)
    : WxStageCanvas3D(stage, rc)
{
}

bool WxStageCanvas::OnUpdate()
{
    WxStageCanvas3D::OnUpdate();

    float dt = 1.0f / 30.0f;
    //clock_t curr_time = clock();
    //if (m_last_time != 0) {
    //    dt = (float)(curr_time - m_last_time) / CLOCKS_PER_SEC;
    //}
    //m_last_time = curr_time;

    static_cast<WxStagePage*>(m_stage)->GetPhysicsMgr().DoSimulationStep(dt);

    return true;
}

}
}