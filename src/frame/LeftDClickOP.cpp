#include "frame/LeftDClickOP.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePage.h"
#include "frame/Application.h"
#include "frame/WxStagePanel.h"

#ifdef MODULE_MASK
#include "mask/WxEditDialog.h"
#endif // MODULE_MASK
#ifdef MODULE_SCALE9
#include "scale9/WxEditDialog.h"
#endif // MODULE_SCALE9

#include <ee0/WxStageCanvas.h>
#include <ee0/EditOpState.h>

#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompMask.h>
#include <node2/CompScale9.h>
#else
#include <entity0/World.h>
#include <entity2/CompMask.h>
#include <entity2/CompScale9.h>
#endif // GAME_OBJ_ECS

namespace
{

void OpenEditDialog(ee0::WxStagePage& stage, ECS_WORLD_PARAM
                    const ee0::GameObj& obj, const ee0::RenderContext& rc,
	                const ee0::WindowContext& wc)
{
	auto stage_panel = eone::Blackboard::Instance()->GetApp()->GetStagePanel();
#ifndef GAME_OBJ_ECS
	if (obj->HasSharedComp<n2::CompMask>())
#else
	if (world.HasComponent<e2::CompMask>(obj))
#endif // GAME_OBJ_ECS
	{
#ifdef MODULE_MASK
		auto& canvas = stage.GetImpl().GetCanvas();
		eone::mask::WxEditDialog dlg(stage_panel, rc, wc, ECS_WORLD_VAR obj);
		dlg.ShowModal();
#endif // MODULE_MASK
	}
#ifndef GAME_OBJ_ECS
	else if (obj->HasSharedComp<n2::CompScale9>())
#else
	else if (world.HasComponent<e2::CompScale9>(obj))
#endif // GAME_OBJ_ECS
	{
#ifdef MODULE_SCALE9
		auto& canvas = stage.GetImpl().GetCanvas();
		eone::scale9::WxEditDialog dlg(stage_panel, rc, wc, ECS_WORLD_VAR obj);
		dlg.ShowModal();
#endif // MODULE_SCALE9
	}
}

}

namespace eone
{

LeftDClickOP::LeftDClickOP(const std::shared_ptr<pt0::Camera>& camera, ee0::WxStagePage& stage,
	                       const ee0::RenderContext& rc, const ee0::WindowContext& wc)
	: ee0::EditOP(camera)
    , m_stage(stage)
    , m_rc(rc)
	, m_wc(wc)
{
}

bool LeftDClickOP::OnMouseLeftDClick(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDClick(x, y)) {
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