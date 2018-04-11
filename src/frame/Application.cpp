#include "frame/Application.h"
#include "frame/WxLibraryPanel.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStagePage.h"
#include "frame/WxStageExtPanel.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"
#include "frame/WxSceneTreePanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/WxToolbarPanel.h"
#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"
#include "frame/GameObjFactory.h"
#include "frame/StagePageType.h"
#include "frame/StagePageFactory.h"
#include "frame/Blackboard.h"
#include "frame/typedef.h"

#include <ee0/CompNodeEditor.h>
#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>
#include <ee0/ConfigFile.h>
#include <ee2/WxStageCanvas.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/Serializer.h>

#include <js/RapidJsonHelper.h>
#include <node0/SceneNode.h>
#include <ns/RegistCallback.h>
#include <facade/Facade.h>
#include <facade/GTxt.h>

#include <boost/filesystem.hpp>

namespace eone
{

Application::Application(wxFrame* frame)
	: m_frame(frame)
	, m_mgr(frame)
	, m_stage(nullptr)
{
	Blackboard::Instance()->InitRenderContext();
	InitSubmodule();
	InitLayout();
	InitCallback();
}

Application::~Application()
{
	m_mgr.UnInit();
}

void Application::LoadFromFile(const std::string& filepath)
{
	if (m_stage->SwitchToPage(filepath)) {
		return;
	}

	auto page = m_stage->GetCurrentStagePage();
	int old_type = page->GetPageType();

	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);
	
	int new_type = PAGE_INVALID;
	std::string new_type_str = doc["comp_type"].GetString();
	if (new_type_str == "n2_complex") {
		//std::string camera = doc["camera"].GetString();
		//if (camera == "2d") {
			new_type = PAGE_SCENE2D;
		//} else if (camera == "3d") {
		//	new_type = PAGE_SCENE3D;
		//}
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

	if (old_type != new_type || !page->GetFilepath().empty()) {
		page = StagePageFactory::Create(new_type, m_stage);
		page->GetSubjectMgr()->NotifyObservers(ee0::MSG_STAGE_PAGE_ON_SHOW);
	}

	auto dir = boost::filesystem::path(filepath).parent_path().string();
	page->LoadFromJson(dir, doc);

	page->SetFilepath(filepath);
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
	facade::Facade::Init();

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
	auto tree      = CreateTreePanel();
	auto detail    = CreateDetailPanel();
	auto toolbar   = CreateToolbarPanel();

	m_mgr.AddPane(library, wxAuiPaneInfo().Name(STR_LIBRARY_PANEL).
		Caption(STR_LIBRARY_PANEL).Left().MinSize(100, 0));
	
	m_mgr.AddPane(stage, wxAuiPaneInfo().Name(STR_STAGE_PANEL).
		Caption(STR_STAGE_PANEL).CenterPane().PaneBorder(false));

	m_mgr.AddPane(stage_ext, wxAuiPaneInfo().Name(STR_STAGE_EXT_PANEL).
		Caption(STR_STAGE_EXT_PANEL).CenterPane().PaneBorder(false));

	m_mgr.AddPane(preview, wxAuiPaneInfo().Name(STR_PREVIEW_PANEL).
		Caption(STR_PREVIEW_PANEL).CenterPane().PaneBorder(false));

	m_mgr.AddPane(tree, wxAuiPaneInfo().Name(STR_TREE_PANEL).
		Caption(STR_TREE_PANEL).Right().Row(1).MinSize(200, 0).PaneBorder(false));

	m_mgr.AddPane(detail, wxAuiPaneInfo().Name(STR_DETAIL_PANEL).
		Caption(STR_DETAIL_PANEL).Right().Row(0).MinSize(300, 0).PaneBorder(false));

	m_mgr.AddPane(toolbar, wxAuiPaneInfo().Name(STR_TOOLBAR_PANEL).
		Caption(STR_TOOLBAR_PANEL).Right().Row(0).MinSize(600, 0).PaneBorder(false));

	m_mgr.Update();
}

void Application::InitCallback()
{
	ns::RegistCallback::Init();
}

wxWindow* Application::CreateLibraryPanel()
{
	auto library = new WxLibraryPanel(m_frame);
	Blackboard::Instance()->SetLiraryPanel(library);
	return library;
}

wxWindow* Application::CreateStagePanel()
{
	m_stage = new WxStagePanel(m_frame);
	m_stage->Freeze();
	Blackboard::Instance()->SetStagePanel(m_stage);

	StagePageFactory::Create(PAGE_SCENE2D, m_stage);
	//StagePageFactory::Create(PAGE_SCALE9, m_stage);
	//StagePageFactory::Create(PAGE_SCRIPT, m_stage);
	//StagePageFactory::Create(PAGE_ANIM, m_stage);
	//StagePageFactory::Create(PAGE_PARTICLE3D, m_stage);

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

	auto canvas = std::make_shared<WxPreviewCanvas>(preview,
		Blackboard::Instance()->GetRenderContext());
	preview->GetImpl().SetCanvas(canvas);
	StagePageFactory::CreatePreviewOP();

	return preview;
}

wxWindow* Application::CreateTreePanel()
{
	auto curr_page = m_stage->GetCurrentStagePage();
	return new WxSceneTreePanel(m_frame, 
		curr_page->GetSubjectMgr(), curr_page->GetEditedObj());
}

wxWindow* Application::CreateDetailPanel()
{
	auto curr_page = m_stage->GetCurrentStagePage();
	return new WxDetailPanel(m_frame, 
		curr_page->GetSubjectMgr(), curr_page->GetEditedObj(), curr_page->GetMoonCtx());
}

wxWindow* Application::CreateToolbarPanel()
{
	auto toolbar = new WxToolbarPanel(m_frame);
	Blackboard::Instance()->SetToolbarPanel(toolbar);
	return toolbar;
}

}