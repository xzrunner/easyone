#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePanel.h"
#include "frame/StagePageType.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"

#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#ifndef GAME_OBJ_ECS
#include <node0/CompAsset.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <ns/CompSerializer.h>
#else
#endif // GAME_OBJ_ECS
#include <moon/Blackboard.h>
#include <moon/SceneGraph.h>
#include <moon/Context.h>

namespace eone
{

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj, LayoutType layout_type)
	: ee0::WxStagePage(parent)
	ECS_WORLD_SELF_ASSIGN
	, m_obj(obj)
	, m_layout_type(layout_type)
{
	m_sub_mgr->RegisterObserver(ee0::MSG_STAGE_PAGE_ON_SHOW, this);

	m_messages.push_back(ee0::MSG_SET_EDITOR_DIRTY);
//	m_messages.push_back(ee0::MSG_STAGE_PAGE_ON_SHOW);
	m_messages.push_back(ee0::MSG_STAGE_PAGE_ON_HIDE);
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	ee0::WxStagePage::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_SET_EDITOR_DIRTY:
		SetEditorDirty(variants);
		break;

	case ee0::MSG_STAGE_PAGE_ON_SHOW:
		SetMoonContext();
		RegisterAllMessages();
		InitSubWindow();
		OnPageInit();
		break;
	case ee0::MSG_STAGE_PAGE_ON_HIDE:
		UnregisterAllMessages();
		break;
	}
}

void WxStagePage::StoreToJson(const std::string& dir, rapidjson::Value& val,
	                          rapidjson::MemoryPoolAllocator<>& alloc) const
{
#ifndef GAME_OBJ_ECS
	ns::CompSerializer::Instance()->ToJson(GetEditedObjComp(), dir, val, alloc);
#else
	// todo ecs
#endif // GAME_OBJ_ECS
	StoreToJsonExt(dir, val, alloc);
}

void WxStagePage::LoadFromJson(const std::string& dir, const rapidjson::Value& val)
{
	m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	m_sub_mgr->NotifyObservers(ee0::MSG_CLEAR_SCENE_NODE);

#ifndef GAME_OBJ_ECS
	ns::CompSerializer::Instance()->FromJson(m_obj, dir, val);
#else
	// todo ecs
#endif // GAME_OBJ_ECS
	LoadFromJsonExt(dir, val);

#ifndef GAME_OBJ_ECS
	auto& casset = m_obj->GetSharedComp<n0::CompAsset>();
	auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
	cbb.SetSize(*m_obj, casset.GetBounding());
#else
	// todo ecs
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::SetEditorDirty(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("dirty");
	GD_ASSERT(var.m_type == ee0::VT_BOOL, "err val");
	auto stage_panel = Blackboard::Instance()->GetStagePanel();
	auto stage_page = stage_panel->GetPage(stage_panel->GetSelection());
	std::string page_name = GetPageName(static_cast<WxStagePage*>(stage_page)->GetPageType());
	if (var.m_val.bl) {
		page_name += "*";
	}
	stage_panel->SetPageText(stage_panel->GetSelection(), page_name);
}

void WxStagePage::RegisterAllMessages()
{
	for (auto& msg : m_messages) {
		m_sub_mgr->RegisterObserver(msg, this);
	}
}

void WxStagePage::UnregisterAllMessages()
{
	for (auto& msg : m_messages) {
		m_sub_mgr->UnregisterObserver(msg, this);
	}
}

void WxStagePage::InitSubWindow()
{
	auto& ui_mgr = Blackboard::Instance()->GetApp()->GetUIManager();
	switch (m_layout_type)
	{
	case LAYOUT_PREVIEW:
		ui_mgr.GetPane(STR_PREVIEW_PANEL).Show();
		ui_mgr.GetPane(STR_WORLD_PANEL).Show();
		ui_mgr.GetPane(STR_DETAIL_PANEL).Show();
		ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Hide();
		ui_mgr.GetPane(STR_TOOLBAR_PANEL).Hide();
		break;
	case LAYOUT_STAGE_EXT:
		ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Show();
		ui_mgr.GetPane(STR_WORLD_PANEL).Show();
		ui_mgr.GetPane(STR_DETAIL_PANEL).Show();
		ui_mgr.GetPane(STR_PREVIEW_PANEL).Hide();
		ui_mgr.GetPane(STR_TOOLBAR_PANEL).Hide();
		break;
	case LAYOUT_TOOLBAR:
		ui_mgr.GetPane(STR_TOOLBAR_PANEL).Show();
		ui_mgr.GetPane(STR_WORLD_PANEL).Hide();
		ui_mgr.GetPane(STR_DETAIL_PANEL).Hide();
		ui_mgr.GetPane(STR_PREVIEW_PANEL).Hide();
		ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Hide();
		break;
	}
	ui_mgr.Update();
}

void WxStagePage::SetMoonContext()
{
	auto bb = moon::Blackboard::Instance();

	bb->SetContext(GetMoonCtx());

	auto scene = bb->GetContext()->GetModuleMgr().GetModule<moon::SceneGraph>(
		moon::Module::M_SCENE_GRAPH);
	scene->SetRoot(m_obj);

	bb->SetSubMgr(m_sub_mgr);
}

}