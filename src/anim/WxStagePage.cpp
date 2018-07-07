#include "anim/WxStagePage.h"
#include "anim/WxTimelinePanel.h"
#include "anim/MessageID.h"
#include "anim/AnimHelper.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/WxStageExtPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee2/WxStageDropTarget.h>

#include <guard/check.h>
#include <logger.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>
#else

#endif // GAME_OBJ_ECS
#include <anim/AnimTemplate.h>

#include <wx/aui/framemanager.h>

namespace eone
{
namespace anim
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, LAYOUT_STAGE_EXT)
{
	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& canim_inst = obj->GetUniqueComp<n2::CompAnimInst>();
	canim_inst.GetPlayCtrl().SetActive(false);
#endif // GAME_OBJ_ECS

	m_messages.push_back(MSG_SET_CURR_FRAME);
	m_messages.push_back(MSG_REFRESH_ANIM_COMP);

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
	case MSG_SET_CURR_FRAME:
		dirty = OnSetCurrFrame(variants);
		break;
	case MSG_REFRESH_ANIM_COMP:
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
	auto panel = Blackboard::Instance()->GetStageExtPanel();
	auto sizer = panel->GetSizer();
	if (sizer) {
		sizer->Clear(true);
	} else {
		sizer = new wxBoxSizer(wxVERTICAL);
	}
	// todo ecs
#ifndef GAME_OBJ_ECS
	auto& canim = m_obj->GetSharedComp<n2::CompAnim>();
	auto& canim_inst = m_obj->GetUniqueComp<n2::CompAnimInst>();
	sizer->Add(new WxTimelinePanel(panel, canim, canim_inst, m_sub_mgr), 0, wxEXPAND);
#endif // GAME_OBJ_ECS
	panel->SetSizer(sizer);
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n2::CompAnim>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::LoadFromFileImpl(const std::string& filepath)
{
	m_sub_mgr->NotifyObservers(MSG_REFRESH_ANIM_COMP);
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
	AnimHelper::UpdateTreePanael(*m_sub_mgr, canim_inst);
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

}
}