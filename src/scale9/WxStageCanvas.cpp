#include "scale9/WxStageCanvas.h"
#include "scale9/ComposeGrids.h"

#include "frame/WxStagePage.h"

#include <SM_Matrix2D.h>
#include <node0/SceneNode.h>
#include <node2/RenderSystem.h>
#include <node2/CompTransform.h>

namespace eone
{
namespace scale9
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage,
	                         const ee0::RenderContext& rc)
	: ee2::WxStageCanvas(stage, &rc)
{
}

void WxStageCanvas::DrawBackground() const
{
	ComposeGrids::Draw();
}

void WxStageCanvas::DrawNodes() const
{
	m_stage->Traverse([&](const n0::SceneNodePtr& node)->bool 
	{
		auto& ctrans = node->GetUniqueComp<n2::CompTransform>();
		auto& trans = ctrans.GetTrans();
		auto old_scale = trans.GetScale();
		trans.SetScale(sm::vec2(1, 1));
		n2::RenderSystem::Draw(node, sm::Matrix2D());
		trans.SetScale(old_scale);
		return true;
	});
}

}
}