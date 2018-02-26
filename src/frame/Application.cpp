#include "Application.h"
#include "frame/WxLibraryPanel.h"
#include "frame/WxStagePanel.h"
#include "frame/WxSceneTreePanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"
#include "frame/Serializer.h"

#include <ee0/CompNodeEditor.h>
#include <ee2/WxStagePage.h>
#include <ee2/WxStageCanvas.h>
#include <ee3/WxStagePage.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/Serializer.h>

#include <node0/SceneNode.h>
#include <ns/RegistCallback.h>
#include <gum/Facade.h>
#include <gum/GTxt.h>

namespace eone
{

Application::Application(wxFrame* frame)
	: m_frame(frame)
	, m_mgr(frame)
{
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
	auto page = m_stage->GetCurrentStagePage();
	if (!page) {
		return;
	}

	Serializer::LoadFromFile(*page, filepath);
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

	Serializer::StoreToFile(*page, _filepath);
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
	m_mgr.AddPane(CreateLibraryPanel(),
		wxAuiPaneInfo().Name("Library").Caption("Library").
		Left().MinSize(wxSize(100, 0)));
	
	m_mgr.AddPane(CreateStagePanel(),
		wxAuiPaneInfo().Name("Stage").Caption("Stage").
		CenterPane().PaneBorder(false));

	m_mgr.AddPane(CreateTreePanel(),
		wxAuiPaneInfo().Name("Tree").Caption("Tree").
		Right().Row(1).MinSize(200, 0).PaneBorder(false));

	m_mgr.AddPane(CreateDetailPanel(),
		wxAuiPaneInfo().Name("Detail").Caption("Detail").
		Right().MinSize(300, 0).PaneBorder(false));

	m_mgr.Update();
}

void Application::InitCallback()
{
	ns::RegistCallback::Init();
}

wxWindow* Application::CreateLibraryPanel()
{
	m_library = new WxLibraryPanel(m_frame);
	return m_library;
}

wxWindow* Application::CreateStagePanel()
{
	m_stage = new WxStagePanel(m_frame);
	Blackboard::Instance()->SetStage(m_stage);
	m_stage->Freeze();

	std::shared_ptr<wxGLContext> gl_ctx = nullptr;
	{
		auto page = new ee2::WxStagePage(m_frame, m_library);
		auto canvas = std::make_shared<ee2::WxStageCanvas>(page);
		gl_ctx = canvas->GetGLContext();
		page->GetImpl().SetCanvas(canvas);
		page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page));

		m_stage->AddPage(page, ("New 2d"));
	}
	{
		auto page = new ee3::WxStagePage(m_frame, m_library);
		auto canvas = std::make_shared<ee3::WxStageCanvas>(page, gl_ctx);
		page->GetImpl().SetCanvas(canvas);
		page->GetImpl().SetEditOP(std::make_shared<ee3::NodeArrangeOP>(*page));

		m_stage->AddPage(page, ("New 3d"));
	}

	m_stage->Thaw();

	return m_stage;
}

wxWindow* Application::CreateTreePanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	m_tree = new WxSceneTreePanel(m_frame, sub_mgr);
	return m_tree;
}

wxWindow* Application::CreateDetailPanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	return new WxDetailPanel(m_frame, sub_mgr);
}

}