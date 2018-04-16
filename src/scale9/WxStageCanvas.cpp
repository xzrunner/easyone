#include "scale9/WxStageCanvas.h"
#include "scale9/ComposeGrids.h"

#include "frame/WxStagePage.h"

#include <SM_Matrix2D.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/RenderSystem.h>
#include <node2/CompTransform.h>
#else
#include <entity2/SysTransform.h>
#include <entity2/SysRender.h>
#endif // GAME_OBJ_ECS

namespace eone
{
namespace scale9
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage,
	                         ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: ee2::WxStageCanvas(stage, ECS_WORLD_VAR &rc)
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
#ifndef GAME_OBJ_ECS
		auto& ctrans = obj->GetUniqueComp<n2::CompTransform>();
		auto old_scale = ctrans.GetTrans().GetScale();
		ctrans.SetScale(*obj, sm::vec2(1, 1));
		n2::RenderSystem::Draw(obj);
		ctrans.SetScale(*obj, old_scale);
#else
		auto old_scale = e2::SysTransform::GetScale(m_world, obj);
		e2::SysTransform::SetScale(m_world, obj, sm::vec2(1, 1));
		e2::SysRender::Draw(m_world, obj);
		e2::SysTransform::SetScale(m_world, obj, old_scale);
#endif // GAME_OBJ_ECS
		return true;
	});
}

}
}