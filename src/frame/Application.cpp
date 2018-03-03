#include "frame/Application.h"
#include "frame/WxLibraryPanel.h"
#include "frame/WxStagePanel.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"
#include "frame/WxSceneTreePanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/NodeSelectOP.h"
#include "frame/Blackboard.h"
#include "frame/Serializer.h"
#include "frame/NodeFactory.h"

#include "scene2d/WxStagePage.h"

#include <ee0/CompNodeEditor.h>
#include <ee2/WxStageCanvas.h>
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
	m_library = new WxLibraryPanel(m_frame);
	return m_library;
}

wxWindow* Application::CreateStagePanel()
{
	m_stage = new WxStagePanel(m_frame);
	Blackboard::Instance()->SetStage(m_stage);
	m_stage->Freeze();
	{
		auto node = NodeFactory::Create(NODE_SCENE2D);
		auto page = new scene2d::WxStagePage(m_frame, m_library, node);
		auto canvas = std::make_shared<ee2::WxStageCanvas>(page);
		m_rc = canvas->GetRenderContext();
		m_wc = canvas->GetWidnowContext();
		page->GetImpl().SetCanvas(canvas);
		page->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*page, m_rc, m_wc));

		m_stage->AddPage(page, ("Scene2D"));
	}
	m_stage->Thaw();

	return m_stage;
}

wxWindow* Application::CreatePreviewPanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	m_preview = new WxPreviewPanel(m_frame, sub_mgr, m_stage->GetCurrentStagePage());

	auto canvas = std::make_shared<WxPreviewCanvas>(m_preview, m_rc);
	m_preview->GetImpl().SetCanvas(canvas);
	auto op = std::make_shared<ee2::CamControlOP>(*canvas->GetCamera(), sub_mgr);
	m_preview->GetImpl().SetEditOP(op);

	return m_preview;
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