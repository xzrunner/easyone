#include "guigraph/WxStageCanvas.h"
#include "guigraph/WxStagePage.h"

#include <blueprint/Node.h>
#include <blueprint/CompNode.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <guilab/Evaluator.h>

namespace eone
{
namespace guigraph
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
                             const ee0::RenderContext& rc)
    : ee2::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
{
    InitGui();
}

bool WxStageCanvas::OnUpdate()
{
    return false;
}
void WxStageCanvas::DrawForeground() const
{
    auto graph_obj = static_cast<WxStagePage*>(m_stage)->GetGraphObj();
    if (!graph_obj) {
        return;
    }

    std::vector<std::shared_ptr<bp::Node>> nodes;

    auto& ccomplex = graph_obj->GetSharedComp<n0::CompComplex>();
    for (auto& child : ccomplex.GetAllChildren())
    {
        if (!child->HasUniqueComp<bp::CompNode>()) {
            continue;
        }
        auto& bp_node = child->GetUniqueComp<bp::CompNode>().GetNode();
        nodes.push_back(bp_node);
    }

    auto& ctx = GetWidnowContext().egui;
    guilab::Evaluator::Draw(nodes, *ctx);
}

}
}