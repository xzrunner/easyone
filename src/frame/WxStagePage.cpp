#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/WxStagePanel.h"
#include "frame/StagePageType.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/AppStyle.h"
#include "frame/WxPreviewPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MsgHelper.h>
#include <ee0/WxStageCanvas.h>

#include <guard/check.h>
#ifndef GAME_OBJ_ECS
#include <node0/CompAsset.h>
#include <node0/CompIdentity.h>
#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompBoundingBox.h>
#include <node2/AABBSystem.h>
#include <node3/CompTransform.h>
#include <node3/CompAABB.h>
#include <node3/AABBSystem.h>
#include <ns/CompSerializer.h>
#include <ns/CompFactory.h>
#else
#endif // GAME_OBJ_ECS
#include <moon/Blackboard.h>
#include <moon/Context.h>
#include <moon/ScriptHelper.h>

#include <boost/filesystem.hpp>

namespace eone
{

WxStagePage::WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj, uint32_t app_style)
	: ee0::WxStagePage(parent)
	ECS_WORLD_SELF_ASSIGN
	, m_obj(obj)
	, m_app_style(app_style)
	, m_backup(m_sub_mgr)
{
    // trigger call RegisterAllMessages()
	m_sub_mgr->RegisterObserver(ee0::MSG_STAGE_PAGE_ON_SHOW, this);

	m_messages.push_back(ee0::MSG_SET_EDITOR_DIRTY);
	m_messages.push_back(ee0::MSG_STAGE_PAGE_ON_HIDE);
}

WxStagePage::~WxStagePage()
{
    m_sub_mgr->UnregisterObserver(ee0::MSG_STAGE_PAGE_ON_SHOW, this);
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
		InitPage();
		break;
	case ee0::MSG_STAGE_PAGE_ON_HIDE:
		UnregisterAllMessages();
		break;
	}
}

void WxStagePage::SetFilepath(const std::string& filepath)
{
	m_filepath = filepath;
	m_backup.SetFilepath(GetBackupPath());
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

void WxStagePage::LoadFromFile(const ur::Device& dev, const std::string& filepath)
{
	m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	m_sub_mgr->NotifyObservers(ee0::MSG_SCENE_NODE_CLEAR);

	n0::CompAssetPtr casset = ns::CompFactory::Instance()->CreateAsset(dev, filepath);
    if (casset)
    {
        if (m_obj->HasSharedComp<n0::CompAsset>()) {
            m_obj->RemoveSharedComp<n0::CompAsset>();
        }
        m_obj->AddSharedCompNoCreate<n0::CompAsset>(casset);
    }
    else
    {
        casset = m_obj->GetSharedCompPtr<n0::CompAsset>();
    }

	// FIXME: reinsert, for send insert msg to other panel
	if (m_obj->HasSharedComp<n0::CompComplex>())
	{
		auto& ccomplex = m_obj->GetSharedComp<n0::CompComplex>();
		auto nodes = ccomplex.GetAllChildren();
		ccomplex.RemoveAllChildren();
		for (auto& node : nodes) {
			ee0::MsgHelper::InsertNode(*m_sub_mgr, node, false);
		}
	}

	LoadFromFileExt(filepath);

	ResetNextID();

	LoadFromBackup();

	ResetNextID();

#ifndef GAME_OBJ_ECS
	if (m_obj->HasUniqueComp<n2::CompBoundingBox>())
	{
		auto& cbb = m_obj->GetUniqueComp<n2::CompBoundingBox>();
		auto aabb = n2::AABBSystem::Instance()->GetBounding(*m_obj);
		cbb.SetSize(*m_obj, aabb);
	}
	else if (m_obj->HasUniqueComp<n3::CompAABB>())
	{
		auto& cbb = m_obj->GetUniqueComp<n3::CompAABB>();
		auto aabb = n3::AABBSystem::GetBounding(*casset);
		// shrink
		if (aabb.Width() > 10 || aabb.Height() > 10 || aabb.Depth() > 10)
		{
			const sm::vec3 scale(0.01f, 0.01f, 0.01f);
			aabb.Scale(scale);

			auto& ctrans = m_obj->GetUniqueComp<n3::CompTransform>();
			ctrans.SetScale(scale);
		}
		cbb.SetAABB(aabb);
	}
#else
	// todo ecs
#endif // GAME_OBJ_ECS

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
}

void WxStagePage::InitPage()
{
    if (!m_inited) {
        OnPageInit();
        m_inited = true;
    }
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

void WxStagePage::InitSubWindow()
{
	auto& ui_mgr = Blackboard::Instance()->GetApp()->GetUIManager();
	if (m_app_style & SHOW_LIBRARY) {
		ui_mgr.GetPane(STR_LIBRARY_PANEL).Show();
	} else {
		ui_mgr.GetPane(STR_LIBRARY_PANEL).Hide();
	}
	if (m_app_style & SHOW_RECORD) {
		ui_mgr.GetPane(STR_RECORD_PANEL).Show();
	} else {
		ui_mgr.GetPane(STR_RECORD_PANEL).Hide();
	}
	if (m_app_style & SHOW_STAGE) {
		ui_mgr.GetPane(STR_STAGE_PANEL).Show();
	} else {
		ui_mgr.GetPane(STR_STAGE_PANEL).Hide();
	}
    if (m_app_style & SHOW_STAGE_EXT) {
        ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Show();
        if (m_app_style & STAGE_EXT_LFET) {
            ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Left().MinSize(500, -1);
        }
        if (m_app_style & STAGE_EXT_RIGHT) {
            ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Right().MinSize(500, -1);
        }
    } else {
	    ui_mgr.GetPane(STR_STAGE_EXT_PANEL).Hide();
    }
	if (m_app_style & SHOW_PREVIEW) {
		ui_mgr.GetPane(STR_PREVIEW_PANEL).Show();
	} else {
		ui_mgr.GetPane(STR_PREVIEW_PANEL).Hide();
		auto preview_panel = Blackboard::Instance()->GetPreviewPanel();
		preview_panel->GetImpl().GetCanvas()->EnableInitiativeUpdate(false);
	}
	if (m_app_style & SHOW_WORLD) {
		ui_mgr.GetPane(STR_WORLD_PANEL).Show();
	} else {
		ui_mgr.GetPane(STR_WORLD_PANEL).Hide();
	}
	if (m_app_style & SHOW_DETAIL) {
		ui_mgr.GetPane(STR_DETAIL_PANEL).Show();
	} else {
		ui_mgr.GetPane(STR_DETAIL_PANEL).Hide();
	}
	if (m_app_style & SHOW_TOOLBAR) {
		ui_mgr.GetPane(STR_TOOLBAR_PANEL).Show();
		if (m_app_style & TOOLBAR_LFET) {
			ui_mgr.GetPane(STR_TOOLBAR_PANEL).Left();
		} else {
			ui_mgr.GetPane(STR_TOOLBAR_PANEL).Right();
		}
	} else {
		ui_mgr.GetPane(STR_TOOLBAR_PANEL).Hide();
	}
	if (m_app_style & SHOW_SCRIPT) {
		ui_mgr.GetPane(STR_SCRIPT_PANEL).Show();
	} else {
		ui_mgr.GetPane(STR_SCRIPT_PANEL).Hide();
	}
	ui_mgr.Update();
}

void WxStagePage::SetMoonContext()
{
	auto bb = moon::Blackboard::Instance();
	bb->SetContext(GetMoonCtx());
	bb->SetStage(this);
	bb->SetSubMgr(m_sub_mgr);
}

void WxStagePage::ResetNextID()
{
	uint32_t max_id = 0;
	Traverse([&](const ee0::GameObj& obj)->bool
	{
		auto& cid = obj->GetUniqueComp<n0::CompIdentity>();
		max_id = std::max(max_id, cid.GetID());
		return true;
	});
	SetNextObjID(max_id + 1);
}

}