#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePage.h"
#include "frame/Application.h"
#include "frame/WxStagePanel.h"

#include "mask/WxEditDialog.h"
#include "scale9/WxEditDialog.h"

#include <ee0/WxStageCanvas.h>
#include <ee0/EditOpState.h>

#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#include <node2/CompScale9.h>
#else
#include <ecsx/World.h>
#include <entity2/CompMask.h>
#include <entity2/CompScale9.h>
#endif // GAME_OBJ_ECS

namespace
{

void OpenEditDialog(ee0::WxStagePage& stage, 
#ifdef GAME_OBJ_ECS
	                ecsx::World& world,
#endif // GAME_OBJ_ECS
                    const ee0::GameObj& obj,
	                const ee0::RenderContext& rc, 
	                const ee0::WindowContext& wc)
{
	auto stage_panel = eone::Blackboard::Instance()->GetApp()->GetStagePanel();
#ifndef GAME_OBJ_ECS
	if (obj->HasSharedComp<n2::CompMask>())
#else
	if (world.HasComponent<e2::CompMask>(obj))
#endif // GAME_OBJ_ECS
	{
		auto& canvas = stage.GetImpl().GetCanvas();
		eone::mask::WxEditDialog dlg(stage_panel, rc, wc, ECS_WORLD_VAR obj);
		dlg.ShowModal();
	}
#ifndef GAME_OBJ_ECS
	else if (obj->HasSharedComp<n2::CompScale9>())
#else
	else if (world.HasComponent<e2::CompScale9>(obj))
#endif // GAME_OBJ_ECS
	{
		auto& canvas = stage.GetImpl().GetCanvas();
		eone::scale9::WxEditDialog dlg(stage_panel, rc, wc, obj);
		dlg.ShowModal();
	}
}

}

namespace eone
{

NodeSelectOP::NodeSelectOP(
#ifdef GAME_OBJ_ECS
	                       ecsx::World& world,
#endif // GAME_OBJ_ECS
	                       WxStagePage& stage,
	                       const ee0::RenderContext& rc,
	                       const ee0::WindowContext& wc)
#ifndef GAME_OBJ_ECS
	: ee2::NodeSelectOP(stage)
#else
	: ee2::NodeSelectOP(world, stage)
	, m_world(world)
#endif // GAME_OBJ_ECS
	, m_rc(rc)
	, m_wc(wc)
{
}

bool NodeSelectOP::OnMouseLeftDClick(int x, int y)
{
	if (ee2::NodeSelectOP::OnMouseLeftDClick(x, y)) {
		return true;
	}

	auto& selection = m_stage.GetSelection();
	if (selection.Size() != 1) {
		return false;
	}

	selection.Traverse([&](const ee0::GameObjWithPos& owp)->bool 
	{
#ifndef GAME_OBJ_ECS
		OpenEditDialog(m_stage, owp.GetNode(), m_rc, m_wc);
#else
		OpenEditDialog(m_stage, m_world, owp, m_rc, m_wc);
#endif // GAME_OBJ_ECS
		return false;
	});

	return false;
}

}