#include "frame/Application.h"
#include "frame/WxLibraryPanel.h"
#include "frame/WxStagePanel.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"
#include "frame/WxSceneTreePanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"
#include "frame/NodeFactory.h"
#include "frame/StagePageType.h"
#include "frame/StagePageFactory.h"
#include "frame/Blackboard.h"

#include "scene2d/WxStagePage.h"

#include <ee0/CompNodeEditor.h>
#include <ee2/WxStageCanvas.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/Serializer.h>

#include <js/RapidJsonHelper.h>
#include <node0/SceneNode.h>
#include <ns/RegistCallback.h>
#include <gum/Facade.h>
#include <gum/GTxt.h>

#include <boost/filesystem.hpp>

namespace eone
{

Application::Application(wxFrame* frame)
	: m_frame(frame)
	, m_mgr(frame)
	, m_stage(nullptr)
{
	InitSubmodule();
	Blackboard::Instance()->InitRenderContext();
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
	}

	if (old_type != new_type || !page->GetFilepath().empty()) {
		page = StagePageFactory::Create(new_type, m_stage);
	}

	auto dir = boost::filesystem::path(filepath).parent_path().string();
	page->LoadFromJson(dir, doc);

	page->SetFilepath(filepath);
	m_frame->SetTitle(filepath);
}

void Application::StoreToFile(const std::string& filepath) const
{
	auto page = m_stage->GetCurrentStagePage();
	if (!page) {
		return;
	}
	
	std::string _filepath = filepath;
	if (_filepath.empty()) {
		_filepath = page->GetFilepath();
	}
	if (_filepath.empty())
	{
		wxFileDialog dlg(Blackboard::Instance()->GetFrame(), wxT("Save"), 
			wxEmptyString, wxEmptyString, "*.json", wxFD_SAVE);
		if (dlg.ShowModal() == wxID_OK) {
			_filepath = dlg.GetPath().ToStdString();		
		}
	}

	page->SetFilepath(_filepath);
	m_frame->SetTitle(_filepath);

	rapidjson::Document doc;
	doc.SetArray();

	auto dir = boost::filesystem::path(_filepath).parent_path().string();

	auto& alloc = doc.GetAllocator();

	page->StoreToJson(dir, doc, alloc);

	js::RapidJsonHelper::WriteToFile(_filepath.c_str(), doc);
}

void Application::Clear()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	sub_mgr.NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);
	sub_mgr.NotifyObservers(ee0::MSG_CLEAR_SCENE_NODE);
}

void Application::InitSubmodule()
{
	CU_VEC<std::pair<CU_STR, CU_STR>> fonts;
	CU_VEC<std::pair<CU_STR, CU_STR>> user_fonts;
	fonts.push_back(std::make_pair("default", "FZCY_GBK.ttf"));
	gum::GTxt::Instance()->Init(fonts, user_fonts);

	gum::Facade::Initialize();
}

void Application::InitLayout()
{
	auto library = CreateLibraryPanel();
	auto stage = CreateStagePanel();
	auto preview = CreatePreviewPanel();
	auto tree = CreateTreePanel();
	auto detail = CreateDetailPanel();

	m_mgr.AddPane(library, wxAuiPaneInfo().Name("Library").Caption("Library").
		Left().MinSize(wxSize(100, 0)));
	
	m_mgr.AddPane(stage, wxAuiPaneInfo().Name("Stage").Caption("Stage").
		CenterPane().PaneBorder(false));

	m_mgr.AddPane(preview, wxAuiPaneInfo().Name("Preview").Caption("Preview").
		CenterPane().PaneBorder(false));

	m_mgr.AddPane(tree, wxAuiPaneInfo().Name("Tree").Caption("Tree").
		Right().Row(1).MinSize(200, 0).PaneBorder(false));

	m_mgr.AddPane(detail, wxAuiPaneInfo().Name("Detail").Caption("Detail").
		Right().MinSize(300, 0).PaneBorder(false));

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

	m_stage->Thaw();

	return m_stage;
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
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	auto tree = new WxSceneTreePanel(m_frame, sub_mgr);
	return tree;
}

wxWindow* Application::CreateDetailPanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	return new WxDetailPanel(m_frame, sub_mgr);
}

}