#include "frame/Application.h"
#include "frame/WxLibraryPanel.h"
#include "frame/WxRecordPanel.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStagePage.h"
#include "frame/WxStageExtPanel.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"
#include "frame/WxWorldPanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/WxToolbarPanel.h"
#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"
#include "frame/GameObjFactory.h"
#include "frame/StagePageType.h"
#include "frame/PanelFactory.h"
#include "frame/Blackboard.h"
#include "frame/typedef.h"
#include "frame/WxScriptPanel.h"

#include "scene2d/WxStagePage.h"
#include "scene3d/WxStagePage.h"
#include "shape2d/WxStagePage.h"
#include "shape3d/WxStagePage.h"
#include "quake/WxStagePage.h"
#include "sgraph/WxStagePage.h"
#include "prototype/WxStagePage.h"

#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee0/ConfigFile.h>
#include <ee2/WxStageCanvas.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/Serializer.h>

#include <js/RapidJsonHelper.h>
#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <ns/RegistCallback.h>
#include <ns/Blackboard.h>
#else
#include <entity2/CompTransform.h>
#include <entity2/CompBoundingBox.h>
#include <entity2/CompComplex.h>
#endif // GAME_OBJ_ECS
#include <facade/Facade.h>
#include <facade/GTxt.h>
#include <moon/moon.h>
#include <sx/ResFileHelper.h>

#include <boost/filesystem.hpp>

namespace
{

extern "C" int luaopen_moon_bp(lua_State* L);
extern "C" int luaopen_moon_sg(lua_State* L);

}

namespace eone
{

Application::Application(wxFrame* frame)
	: m_frame(frame)
	, m_mgr(frame)
	, m_stage(nullptr)
{
	Blackboard::Instance()->InitRenderContext();
	InitSubmodule();
#ifdef GAME_OBJ_ECS
	InitWorld();
#endif // GAME_OBJ_ECS
	InitLayout();
	InitCallback();
}

void Application::LoadFromFile(const std::string& filepath)
{
	if (m_stage->SwitchToPage(filepath)) {
		return;
	}

	auto page = m_stage->GetCurrentStagePage();
	int old_type = page->GetPageType();

	int new_type = PAGE_INVALID;

	auto type = sx::ResFileHelper::Type(filepath);
	switch (type)
	{
	case sx::RES_FILE_JSON:
	{
		rapidjson::Document doc;
		js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

		std::string new_type_str = doc[ns::CompSerializer::COMP_TYPE_NAME].GetString();
		if (new_type_str == "n0_complex") {
			assert(doc.HasMember("page_type"));
			auto type = doc["page_type"].GetString();
			if (type == scene2d::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SCENE2D;
			} else if (type == scene3d::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SCENE3D;
			} else if (type == shape2d::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SHAPE2D;
			} else if (type == shape3d::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SHAPE3D;
			} else if (type == quake::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_QUAKE;
			} else if (type == sgraph::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_SHADER_GRAPH;
			} else if (type == prototype::WxStagePage::PAGE_TYPE) {
				new_type = PAGE_PROTOTYPING;
			}
		} else if (new_type_str == "n2_scale9") {
			new_type = PAGE_SCALE9;
		} else if (new_type_str == "n2_mask") {
			new_type = PAGE_MASK;
		} else if (new_type_str == "n2_mesh") {
			new_type = PAGE_MESH;
		} else if (new_type_str == "n2_anim") {
			new_type = PAGE_ANIM;
		} else if (new_type_str == "n2_particle3d") {
			new_type = PAGE_PARTICLE3D;
		}
	}
		break;
	case sx::RES_FILE_MODEL:
		if (old_type == PAGE_MODEL) {
			new_type = PAGE_MODEL;
		} else if (old_type == PAGE_ANIM3) {
			new_type = PAGE_ANIM3;
		} else {
			new_type = PAGE_MODEL;
		}
		break;
	case sx::RES_FILE_MAP:
		new_type = PAGE_QUAKE;
		break;
	}

	if (old_type != new_type) {
		page = PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR new_type, m_stage);
		page->GetSubjectMgr()->NotifyObservers(ee0::MSG_STAGE_PAGE_ON_SHOW);
	}

	page->SetFilepath(filepath);
	page->LoadFromFile(filepath);

	m_frame->SetTitle(filepath);
}

void Application::StoreToFile(const std::string& filepath) const
{
	auto path = m_stage->StoreCurrPage(filepath);
	GD_ASSERT(!path.empty(), "err path");
	m_frame->SetTitle(path);
}

void Application::Clear()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	sub_mgr->NotifyObservers(ee0::MSG_CLEAR_SCENE_NODE);
}

void Application::InitSubmodule()
{
	facade::Facade::Instance()->AddInitCB([] {
		moon_add_module("moon.bp", luaopen_moon_bp);
		moon_add_module("moon.sg", luaopen_moon_sg);
	});
	facade::Facade::Instance()->Init();

	auto cfg = ee0::ConfigFile::Instance();
	facade::GTxt::Instance()->LoadFonts(cfg->GetFonts(), cfg->GetUserFonts());

	//gum::Facade::Initialize();
}

void Application::InitLayout()
{
	auto library   = CreateLibraryPanel();
	auto stage     = CreateStagePanel();
	auto stage_ext = CreateStageExtPanel();
	auto preview   = CreatePreviewPanel();
	auto world     = CreateWorldPanel();
	auto detail    = CreateDetailPanel();
	auto toolbar   = CreateToolbarPanel();
	auto record    = CreateRecordPanel();
	auto script    = CreateScriptPanel();

	m_mgr.AddPane(library, wxAuiPaneInfo().Name(STR_LIBRARY_PANEL).
		Caption(STR_LIBRARY_PANEL).Left().MinSize(150, 0));
	m_mgr.AddPane(record, wxAuiPaneInfo().Name(STR_RECORD_PANEL).
		Caption(STR_RECORD_PANEL).Left());

	//m_mgr.AddPane(stage, wxAuiPaneInfo().Name(STR_STAGE_PANEL).
	//	Caption(STR_STAGE_PANEL).CenterPane().Row(1).PaneBorder(false));
	m_mgr.AddPane(stage, wxAuiPaneInfo().Name(STR_STAGE_PANEL).
		Caption(STR_STAGE_PANEL).CenterPane().PaneBorder(false));

	m_mgr.AddPane(stage_ext, wxAuiPaneInfo().Name(STR_STAGE_EXT_PANEL).
		Caption(STR_STAGE_EXT_PANEL).CenterPane().PaneBorder(false));

	m_mgr.AddPane(preview, wxAuiPaneInfo().Name(STR_PREVIEW_PANEL).
		Caption(STR_PREVIEW_PANEL).CenterPane().PaneBorder(false));

	m_mgr.AddPane(world, wxAuiPaneInfo().Name(STR_WORLD_PANEL).
		Caption(STR_WORLD_PANEL).Right().Row(1).MinSize(200, 0).PaneBorder(false));

	m_mgr.AddPane(detail, wxAuiPaneInfo().Name(STR_DETAIL_PANEL).
		Caption(STR_DETAIL_PANEL).Right().Row(0).MinSize(300, 0).PaneBorder(false));

	m_mgr.AddPane(script, wxAuiPaneInfo().Name(STR_SCRIPT_PANEL).
		Caption(STR_SCRIPT_PANEL).Right().Row(0).MinSize(300, 0).PaneBorder(false));

	m_mgr.AddPane(toolbar, wxAuiPaneInfo().Name(STR_TOOLBAR_PANEL).
		Caption(STR_TOOLBAR_PANEL).Right().Row(0).MinSize(300, 0).PaneBorder(false));

	m_mgr.Update();
}

void Application::InitCallback()
{
#ifndef GAME_OBJ_ECS
	ns::RegistCallback::Init();
	ns::Blackboard::Instance()->SetGenNodeIdFunc([]()->uint32_t {
		auto stage = static_cast<WxStagePage*>(
			Blackboard::Instance()->GetStagePanel()->GetCurrentPage());
		return stage->FetchObjID();
	});
#endif // GAME_OBJ_ECS
}

#ifdef GAME_OBJ_ECS
void Application::InitWorld()
{
	m_world.SetCompStorage<e2::CompPosition>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompAngle>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompScale>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompShear>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompOffset>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<e2::CompLocalMat>(e0::COMP_STORAGE_DENSE);

	m_world.SetCompStorage<e2::CompBoundingBox>(e0::COMP_STORAGE_DENSE);
	m_world.SetCompStorage<ee0::CompEntityEditor>(e0::COMP_STORAGE_DENSE);

	m_world.SetCompStorage<e2::CompComplex>(e0::COMP_STORAGE_SPARSE);
}
#endif // GAME_OBJ_ECS

wxWindow* Application::CreateLibraryPanel()
{
	auto library = new WxLibraryPanel(m_frame);
	Blackboard::Instance()->SetLiraryPanel(library);
	return library;
}

wxWindow* Application::CreateRecordPanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	return new WxRecordPanel(m_frame, sub_mgr);
}

wxWindow* Application::CreateStagePanel()
{
	m_stage = new WxStagePanel(m_frame);
	m_stage->Freeze();
	Blackboard::Instance()->SetStagePanel(m_stage);

	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SCENE2D, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SCENE3D, m_stage);

	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SCALE9, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SCRIPT, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_ANIM, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_PARTICLE3D, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SHAPE2D, m_stage);
	PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SHAPE3D, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_MESH, m_stage);

	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_MODEL, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_ANIM3, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_SHADER_GRAPH, m_stage);
	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_PROTOTYPING, m_stage);

	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_QUAKE, m_stage);

	//PanelFactory::CreateStagePage(ECS_WORLD_SELF_VAR PAGE_BLUEPRINT, m_stage);

	m_stage->Thaw();

	return m_stage;
}

wxWindow* Application::CreateStageExtPanel()
{
	auto stage_ext = new WxStageExtPanel(m_frame);
	Blackboard::Instance()->SetStageExtPanel(stage_ext);
	return stage_ext;
}

wxWindow* Application::CreatePreviewPanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	auto preview = new WxPreviewPanel(m_frame, sub_mgr, m_stage->GetCurrentStagePage());
	Blackboard::Instance()->SetPreviewPanel(preview);

	auto canvas = std::make_shared<WxPreviewCanvas>(
		preview, ECS_WORLD_SELF_VAR Blackboard::Instance()->GetRenderContext());
	preview->GetImpl().SetCanvas(canvas);
	PanelFactory::CreatePreviewOP(
#ifdef GAME_OBJ_ECS
		m_world
#endif // GAME_OBJ_ECS
	);

	return preview;
}

wxWindow* Application::CreateWorldPanel()
{
	auto curr_page = m_stage->GetCurrentStagePage();
	return new WxWorldPanel(m_frame, curr_page->GetSubjectMgr(),
		ECS_WORLD_SELF_VAR curr_page->GetEditedObj());
}

wxWindow* Application::CreateDetailPanel()
{
	auto curr_page = m_stage->GetCurrentStagePage();
	return new WxDetailPanel(m_frame, curr_page->GetSubjectMgr(),
		ECS_WORLD_SELF_VAR curr_page->GetEditedObj(), curr_page->GetMoonCtx());
}

wxWindow* Application::CreateToolbarPanel()
{
	auto toolbar = new WxToolbarPanel(m_frame);
	Blackboard::Instance()->SetToolbarPanel(toolbar);
	return toolbar;
}

wxWindow* Application::CreateScriptPanel()
{
	return new WxScriptPanel(m_frame);
}

}