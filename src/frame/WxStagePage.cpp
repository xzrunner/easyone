#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePanel.h"
#include "frame/StagePageType.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"

#include <ee0/SubjectMgr.h>

#include <guard/check.h>
#include <js/RapidJsonHelper.h>
#include <sx/ResFileHelper.h>
#ifndef GAME_OBJ_ECS
#include <node0/CompAsset.h>
#include <node0/CompIdentity.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <node2/AABBSystem.h>
#include <node3/CompModel.h>
#include <node3/CompTransform.h>
#include <ns/CompSerializer.h>
#include <ns/NodeFactory.h>
#else
#endif // GAME_OBJ_ECS
#include <moon/Blackboard.h>
#include <moon/Context.h>
#include <moon/ScriptHelper.h>

#include <boost/filesystem.hpp>

namespace eone
{

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj, LayoutType layout_type)
	: ee0::WxStagePage(parent)
	ECS_WORLD_SELF_ASSIGN
	, m_obj(obj)
	, m_layout_type(layout_type)
	, m_backup(m_sub_mgr)
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

	m_backup.Clear();
}

void WxStagePage::LoadFromFile(const std::string& filepath)
{
	m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	m_sub_mgr->NotifyObservers(ee0::MSG_CLEAR_SCENE_NODE);

	bool is_2d = true;

	auto type = sx::ResFileHelper::Type(filepath);
	switch (type)
	{
	case sx::RES_FILE_IMAGE:
		ns::NodeFactory::CreateNodeAssetComp(m_obj, filepath);
		break;
	case sx::RES_FILE_JSON:
		{
			auto dir = boost::filesystem::path(filepath).parent_path().string();

			rapidjson::Document doc;
			js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

	#ifndef GAME_OBJ_ECS
			ns::CompSerializer::Instance()->FromJson(m_obj, dir, doc);
	#else
			// todo ecs
	#endif // GAME_OBJ_ECS
			LoadFromJsonExt(dir, doc);
		}
		break;
	case sx::RES_FILE_MODEL:
		ns::NodeFactory::CreateNodeAssetComp(m_obj, filepath);
		is_2d = false;
		break;
	}

	ResetNextID();

	LoadFromBackup();

	ResetNextID();

#ifndef GAME_OBJ_ECS
	if (is_2d)
	{
		auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
		auto aabb = n2::AABBSystem::GetBounding(
			m_obj->GetSharedComp<n0::CompAsset>());
		cbb.SetSize(*m_obj, aabb);
	}
#else
	// todo ecs
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::SetFilepath(const std::string& filepath)
{
	m_filepath = filepath;
	m_backup.SetFilepath(GetBackupPath());
}

std::string WxStagePage::GetBackupPath() const
{
	return m_filepath + ".backup";
}

void WxStagePage::LoadFromBackup()
{
	auto filepath = GetBackupPath();
	if (boost::filesystem::exists(filepath))
	{
		auto L = moon::Blackboard::Instance()->GetContext()->GetState();
		auto err = moon::ScriptHelper::DoFile(L, filepath.c_str());
	}
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
	bb->SetSubMgr(m_sub_mgr);
}

void WxStagePage::ResetNextID()
{
	uint32_t max_id = 0;
	Traverse([&](const ee0::GameObj& obj)->bool
	{
		auto& ceditor = obj->GetUniqueComp<ee0::CompNodeEditor>();
		max_id = std::max(max_id, ceditor.GetID());
		return true;
	});
	SetNextObjID(max_id + 1);
}

}