#include "ittgraph/WxStageCanvas.h"

#ifdef MODULE_ITTGRAPH

#include "ittgraph/WxStagePage.h"

#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <intention/Evaluator.h>
#include <intention/Blackboard.h>
#include <intention/Node.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node3/RenderSystem.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>
#include <everything/Evaluator.h>
#include <everything/Node.h>

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

    pt0::RenderContext rc;
    rc.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    if (m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        auto persp = std::static_pointer_cast<pt3::PerspCam>(m_camera);
        rc.AddVar(
            pt3::MaterialMgr::PositionUniforms::cam_pos.name,
            pt0::RenderVariant(persp->GetPos())
        );
    }
    auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    assert(wc);
    rc.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    rc.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

    auto graph_obj = static_cast<WxStagePage*>(m_stage)->GetGraphObj();
    auto& ccomplex = graph_obj->GetSharedComp<n0::CompComplex>();
    for (auto& obj : ccomplex.GetAllChildren())
    {
        if (!obj->HasUniqueComp<bp::CompNode>()) {
            continue;
        }

        auto& bp_node = obj->GetUniqueComp<bp::CompNode>().GetNode();
        if (!bp_node->get_type().is_derived_from<itt::Node>()) {
            continue;
        }

        auto itt_node = std::static_pointer_cast<itt::Node>(bp_node);
        if (!itt_node->GetDisplay()) {
            continue;
        }

        auto evt_node = eval->QueryEvtNode(bp_node.get());
        if (!evt_node) {
            continue;
        }

        auto sn = evt_node->GetSceneNode();
        if (!sn) {
            continue;
        }

        pt3::RenderParams rp;

        // draw face
        rp.type = pt3::RenderParams::DRAW_MESH;
        n3::RenderSystem::Draw(*sn, rp, rc);

        // draw edge
        rp.type = pt3::RenderParams::DRAW_BORDER_MESH;
        n3::RenderSystem::Draw(*sn, rp, rc);
    }
}

}
}

#endif // MODULE_ITTGRAPH