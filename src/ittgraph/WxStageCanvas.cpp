#include "ittgraph/WxStageCanvas.h"

#ifdef MODULE_ITTGRAPH

#include "ittgraph/WxStagePage.h"

#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <intention/Evaluator.h>
#include <intention/Blackboard.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
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

void WxStageCanvas::DrawForeground3D() const
{
    auto eval = itt::Blackboard::Instance()->GetEval();
    if (eval) {
        evt::Evaluator::Draw(eval->GetBackNodes());
    }
}

}
}

#endif // MODULE_ITTGRAPH