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
	m_stage->Traverse([&](const ee0::GameObj& obj)->bool 
	{
		auto& ctrans = obj->GetUniqueComp<n2::CompTransform>();
		auto old_scale = ctrans.GetTrans().GetScale();
		ctrans.SetScale(*obj, sm::vec2(1, 1));
		n2::RenderSystem::Draw(obj);
		ctrans.SetScale(*obj, old_scale);
		return true;
	});
}

}
}