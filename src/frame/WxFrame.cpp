#include "frame/WxFrame.h"
#include "frame/Application.h"
#include "frame/StagePageType.h"
#include "frame/PanelFactory.h"
#include "frame/WxSettingsDialog.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"

#include <ee0/WxListSelectDlg.h>
#include <ee0/SubjectMgr.h>

#include <moon/Blackboard.h>

namespace eone
{

BEGIN_EVENT_TABLE(WxFrame, ee0::WxFrame)
    EVT_MENU(ID_SKYBOX, WxFrame::OnSetSkybox)
END_EVENT_TABLE()

WxFrame::WxFrame()
	: ee0::WxFrame("EasyOne", wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE)
{
	moon::Blackboard::Instance()->SetWindow(this);

    auto menu_bar = GetMenuBar();
    menu_bar->Append(InitToolsBar(), "&Tools");
}

WxFrame::~WxFrame()
{
    auto& ui_mgr = Blackboard::Instance()->GetApp()->GetUIManager();
    ui_mgr.UnInit();
}

void WxFrame::OnNew(wxCommandEvent& event)
{
	struct PageItemData : public wxTreeItemData
	{
		PageItemData(int type)
			: type(type) {}

		int type;

	}; // PageItemData

	const std::vector<std::pair<std::string, wxTreeItemData*>> PAGE_LIST =
	{
		{ "Scene2D",         new PageItemData(eone::PAGE_SCENE2D) },
		{ "Scene3D",         new PageItemData(eone::PAGE_SCENE3D) },

		{ "Scale9",          new PageItemData(eone::PAGE_SCALE9) },
		{ "Mask",            new PageItemData(eone::PAGE_MASK) },
		{ "Mesh",            new PageItemData(eone::PAGE_MESH) },
		{ "Anim",            new PageItemData(eone::PAGE_ANIM) },
		{ "Particle3d",      new PageItemData(eone::PAGE_PARTICLE3D) },
		{ "Shape2d",         new PageItemData(eone::PAGE_SHAPE2D) },
		{ "Shape3d",         new PageItemData(eone::PAGE_SHAPE3D) },
        { "CAD",             new PageItemData(eone::PAGE_CAD) },

		{ "Scale9",          new PageItemData(eone::PAGE_SCALE9) },

		{ "ShaderLab",       new PageItemData(eone::PAGE_SHADER_GRAPH) },
		{ "Prototyping",     new PageItemData(eone::PAGE_PROTOTYPING) },
        { "ShaderGraph",     new PageItemData(eone::PAGE_SHADER_GRAPH2) },
        { "RenderGraph",     new PageItemData(eone::PAGE_RENDER_GRAPH) },
        { "RayGraph",        new PageItemData(eone::PAGE_RAY_GRAPH) },
		{ "GIGraph",         new PageItemData(eone::PAGE_GI_GRAPH) },
        { "GuiGraph",        new PageItemData(eone::PAGE_GUI_GRAPH) },
        { "PbrGraph",        new PageItemData(eone::PAGE_PBR_GRAPH) },
        { "HdiSop",          new PageItemData(eone::PAGE_HDI_SOP) },
        { "HdiVop",          new PageItemData(eone::PAGE_HDI_VOP) },
        { "Architectural",   new PageItemData(eone::PAGE_ARCHITECTURAL) },
        { "Terrain",         new PageItemData(eone::PAGE_TERRAIN) },
        { "City",            new PageItemData(eone::PAGE_CITY) },
        { "Geometry",        new PageItemData(eone::PAGE_GEOMETRY) },
        { "TouchDesigner",   new PageItemData(eone::PAGE_TOUCH_DESIGNER) },
        { "VisionGraph",     new PageItemData(eone::PAGE_VISION_LAB) },
        { "Material",        new PageItemData(eone::PAGE_MATERIAL) },
        { "EditOp",          new PageItemData(eone::PAGE_EDITOP) },
        { "Task",            new PageItemData(eone::PAGE_TASK) },
		{ "VFX",             new PageItemData(eone::PAGE_VFX_GRAPH) },

		{ "Script...",       new PageItemData(eone::PAGE_SCRIPT) },
		{ "Blueprint",       new PageItemData(eone::PAGE_BLUEPRINT) },

		{ "Quake",           new PageItemData(eone::PAGE_QUAKE) },
	};

	ee0::WxListSelectDlg dlg(this, "New page", PAGE_LIST, wxGetMousePosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	auto app = std::static_pointer_cast<Application>(m_app);
	auto type = static_cast<PageItemData*>(dlg.GetSelected())->type;
    auto dev = Blackboard::Instance()->GetRenderDevice();
#ifndef GAME_OBJ_ECS
	auto page = PanelFactory::CreateStagePage(*dev, type, app->GetStagePanel());
#else
	auto page = PanelFactory::Create(app->GetWorld(), type, app->GetStagePanel());
#endif // GAME_OBJ_ECS
    app->GetStagePanel()->AddNewPage(page, GetPageName(page->GetPageType()));

	WxStagePage* curr_page = static_cast<WxStagePage*>(app->GetStagePanel()->GetPage(app->GetStagePanel()->GetSelection()));
	curr_page->GetSubjectMgr()->NotifyObservers(ee0::MSG_STAGE_PAGE_ON_SHOW);
}

void WxFrame::OnOpen(wxCommandEvent& event)
{
	wxFileDialog dlg(this, wxT("Open"), wxEmptyString, wxEmptyString);
	if (dlg.ShowModal() == wxID_OK) {
		m_app->LoadFromFile(dlg.GetPath().ToStdString());
	}
}

void WxFrame::OnSettings(wxCommandEvent& event)
{
	WxSettingsDialog dlg(this);
	dlg.ShowModal();
}

wxMenu* WxFrame::InitToolsBar()
{
    wxMenu* menu = new wxMenu;
    menu->Append(ID_SKYBOX, wxT("Skybox"), wxT("Skybox"));
    return menu;
}

void WxFrame::OnSetSkybox(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("Open"), wxEmptyString, wxEmptyString, "*.hdr");
    if (dlg.ShowModal() == wxID_OK) {
        auto stage_page = static_cast<WxStagePage*>(std::static_pointer_cast<Application>(m_app)->GetStagePanel()->GetCurrentPage());
        stage_page->OnSetSkybox(dlg.GetPath().ToStdString());
    }
}

}