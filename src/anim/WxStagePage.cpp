#include "anim/WxStagePage.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/WxStageSubPanel.h"
#include "frame/AppStyle.h"
#include "frame/GameObjFactory.h"
#include "frame/MessageID.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <logger.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>
#else

#endif // GAME_OBJ_ECS
#include <anim/AnimTemplate.h>
#include <eanim/MessageID.h>
#include <eanim/AnimHelper.h>
#include <eanim/WxTimelinePanel.h>
#include <eanim/Callback.h>

#include <wx/aui/framemanager.h>

namespace eone
{
namespace anim
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_LIBRARY | SHOW_RECORD | SHOW_STAGE | SHOW_STAGE_EXT | SHOW_WORLD | SHOW_DETAIL | SHOW_SCRIPT)
{
	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& canim_inst = obj->GetUniqueComp<n2::CompAnimInst>();
	canim_inst.GetPlayCtrl().SetActive(false);
#endif // GAME_OBJ_ECS

	m_messages.push_back(eanim::MSG_SET_CURR_FRAME);
	m_messages.push_back(eanim::MSG_REFRESH_ANIM_COMP);

	if (library) {
		SetDropTarget(new ee2::WxStageDropTarget(ECS_WORLD_VAR library, this));
	}
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	bool dirty = false;
	switch (msg)
	{
    case ee0::MSG_STAGE_PAGE_ON_SHOW:
        m_timeline->Show();
        m_timeline->GetParent()->Layout();
        break;
    case ee0::MSG_STAGE_PAGE_ON_HIDE:
        m_timeline->Hide();
        m_timeline->GetParent()->Layout();
        break;

	case eanim::MSG_SET_CURR_FRAME:
		dirty = OnSetCurrFrame(variants);
		break;
	case eanim::MSG_REFRESH_ANIM_COMP:
		dirty = OnRefreshAnimComp();
		break;
	}

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}
}

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
	                       const ee0::VariantSet& variants, bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY) {
		// todo ecs
#ifndef GAME_OBJ_ECS
		m_obj->GetSharedComp<n2::CompAnim>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW:
		func(m_obj);
		break;
	case TRAV_DRAW_PREVIEW:
		func(m_obj);
		break;
		// todo ecs
#ifndef GAME_OBJ_ECS
	default:
		m_obj->GetSharedComp<n2::CompAnim>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
	}
}

void WxStagePage::OnPageInit()
{
	auto& canim = m_obj->GetSharedComp<n2::CompAnim>();
	auto& canim_inst = m_obj->GetUniqueComp<n2::CompAnimInst>();

	// set callback
	eanim::Callback::Funs funs;
	funs.refresh_all_nodes = [&]() {
		RefreshAllNodes();
	};
	funs.get_all_layers = [&]()->const std::vector<::anim::LayerPtr>& {
		return canim.GetAllLayers();
	};
	funs.add_layer = [&](::anim::LayerPtr& layer) {
		canim.AddLayer(layer);
	};
	funs.swap_layer = [&](int from, int to) {
		const_cast<n2::CompAnim&>(canim).SwapLayers(from, to);
	};
	funs.remove_all_layers = [&]()->bool {
		return canim.RemoveAllLayers();
	};
	eanim::Callback::RegisterCallback(funs);

    assert(!m_timeline);
    auto stage_ext_panel = Blackboard::Instance()->GetStageExtPanel();
    m_timeline = stage_ext_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new eanim::WxTimelinePanel(parent, m_sub_mgr, canim_inst.GetPlayCtrl());
    }, wxVERTICAL);
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n2::CompAnim>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::LoadFromFileExt(const std::string& filepath)
{
	m_sub_mgr->NotifyObservers(eanim::MSG_REFRESH_ANIM_COMP);
}

bool WxStagePage::OnSetCurrFrame(const ee0::VariantSet& variants)
{
#ifndef GAME_OBJ_ECS
	auto var = variants.GetVariant("frame");
	GD_ASSERT(var.m_type == ee0::VT_INT, "err frame");
	int frame = var.m_val.l;

	auto& canim = m_obj->GetSharedComp<n2::CompAnim>();
	frame = std::min(frame, canim.GetAnimTemplate()->GetMaxFrameIdx());

	auto& canim_inst = m_obj->GetUniqueComp<n2::CompAnimInst>();
	bool ret = canim_inst.SetFrame(frame);
	RefreshAllNodes();
	return ret;
#else
	// todo ecs
	return false;
#endif // GAME_OBJ_ECS
}

bool WxStagePage::OnRefreshAnimComp()
{
	LOGI("refresh anim comp");
	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& canim = m_obj->GetSharedCompPtr<n2::CompAnim>();
	canim->GetAnimTemplate()->Build(canim->GetAllLayers());
#endif // GAME_OBJ_ECS
	return true;
}

void WxStagePage::RefreshAllNodes()
{
	// todo ecs
#ifndef GAME_OBJ_ECS
	auto root = GameObjFactory::Create(GAME_OBJ_COMPLEX2D);
	auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
	auto& canim_inst = m_obj->GetUniqueComp<n2::CompAnimInst>();
	canim_inst.TraverseCurrNodes([&](const ee0::GameObj& obj)->bool
	{
		ccomplex.AddChild(obj);
		return true;
	});

	ee0::VariantSet vars;

	ee0::Variant var;
	var.m_type = ee0::VT_PVOID;
	var.m_val.pv = &root;
	vars.SetVariant("obj", var);

	m_sub_mgr->NotifyObservers(MSG_TREE_PANEL_REBUILD, vars);
#endif // GAME_OBJ_ECS
}

}
}