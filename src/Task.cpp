#include "Task.h"
#include "LibraryPanel.h"
#include "StagePanel.h"
#include "SceneTreePanel.h"
#include "DetailPanel.h"

#include <ee0/CompNodeEditor.h>
#include <ee2/WxStagePage.h>
#include <ee2/WxStageCanvas.h>
#include <ee2/NodeSelectOP.h>
#include <ee3/WxStagePage.h>
#include <ee3/WxStageCanvas.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/Serializer.h>

#include <node0/SceneNode.h>
#include <node3/ComponentFactory.h>
#include <gum/Facade.h>
#include <gum/GTxt.h>

namespace eone
{

Task::Task(wxFrame* frame)
	: m_frame(frame)
{
	m_mgr.SetManagedWindow(frame);

	InitSubmodule();
	InitLayout();
	InitCallback();
}

void Task::InitSubmodule()
{
	CU_VEC<std::pair<CU_STR, CU_STR>> fonts;
	CU_VEC<std::pair<CU_STR, CU_STR>> user_fonts;
	fonts.push_back(std::make_pair("default", "FZCY_GBK.ttf"));
	gum::GTxt::Instance()->Init(fonts, user_fonts);

	gum::Facade::Initialize();
}

void Task::InitLayout()
{
	m_mgr.AddPane(CreateLibraryPanel(),
		wxAuiPaneInfo().Name("Library").Caption("Library").
		Left().MinSize(wxSize(100, 0)));
	
	m_mgr.GetPane("Stage");

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

void Task::InitCallback()
{
	n3::ComponentFactory::Instance()->AddCreator(ee0::CompNodeEditor::TYPE_NAME,
		[](n0::SceneNodePtr& node, const rapidjson::Value& val)
	{
		auto& comp = node->AddComponent<ee0::CompNodeEditor>();
		comp.LoadFromJson(val);
	});
}

wxWindow* Task::CreateLibraryPanel()
{
	m_library = new LibraryPanel(m_frame);
	return m_library;
}

wxWindow* Task::CreateStagePanel()
{
	m_stage = new StagePanel(m_frame);
	m_stage->Freeze();

	std::shared_ptr<wxGLContext> gl_ctx = nullptr;
	{
		auto page = new ee2::WxStagePage(m_frame, m_library);
		auto canvas = std::make_shared<ee2::WxStageCanvas>(page);
		gl_ctx = canvas->GetGLContext();
		page->GetImpl().SetCanvas(canvas);
		page->GetImpl().SetEditOP(std::make_shared<ee2::NodeSelectOP>(*page));

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

wxWindow* Task::CreateTreePanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	m_tree = new SceneTreePanel(m_frame, sub_mgr);
	return m_tree;
}

wxWindow* Task::CreateDetailPanel()
{
	auto& sub_mgr = m_stage->GetCurrentStagePage()->GetSubjectMgr();
	return new DetailPanel(m_frame, sub_mgr);
}

}