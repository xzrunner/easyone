#include "ittgraph/WxStageCanvas.h"

#ifdef MODULE_ITTGRAPH

#include "ittgraph/WxStagePage.h"

#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <intention/Evaluator.h>
#include <intention/Blackboard.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>
#include <everything/Evaluator.h>

namespace eone
{
namespace ittgraph
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
                             const ee0::RenderContext& rc)
    : ee3::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
{
}

bool WxStageCanvas::OnUpdate()
{
    return false;
}

void WxStageCanvas::DrawBackground3D() const
{
    ee3::WxStageCanvas::DrawBackgroundGrids(10.0f, 0.2f);
}

void WxStageCanvas::DrawForeground3D() const
{
    auto eval = itt::Blackboard::Instance()->GetEval();
    if (!eval) {
        return;
    }

    pt0::RenderContext ctx;
    ctx.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    if (m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        auto persp = std::static_pointer_cast<pt3::PerspCam>(m_camera);
        ctx.AddVar(
            pt3::MaterialMgr::PositionUniforms::cam_pos.name,
            pt0::RenderVariant(persp->GetPos())
        );
    }
    auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    assert(wc);
    ctx.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    ctx.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

    evt::Evaluator::Draw(ctx, eval->GetBackNodes());
}

}
}

#endif // MODULE_ITTGRAPH