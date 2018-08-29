#include "frame/WxFrame.h"
#include "frame/Application.h"
#include "frame/StagePageType.h"
#include "frame/StagePageFactory.h"
#include "frame/WxSettingsDialog.h"
#include "frame/WxStagePanel.h"
#include "frame/WxStagePage.h"

#include <ee0/WxListSelectDlg.h>
#include <ee0/SubjectMgr.h>

#include <moon/Blackboard.h>

namespace
{

static const std::vector<std::pair<uint32_t, std::string>> PAGE_LIST =
{
	std::make_pair(eone::PAGE_SCENE2D,    "Scene2D"),
	std::make_pair(eone::PAGE_SCENE3D,    "Scene3D"),

	std::make_pair(eone::PAGE_SCALE9,     "Scale9"),
	std::make_pair(eone::PAGE_MASK,       "Mask"),
	std::make_pair(eone::PAGE_MESH,       "Mesh"),
	std::make_pair(eone::PAGE_ANIM,       "Anim"),
	std::make_pair(eone::PAGE_PARTICLE3D, "Particle3d"),

	std::make_pair(eone::PAGE_SCALE9,     "Scale9"),

	std::make_pair(eone::PAGE_MATERIAL,   "Material"),

	std::make_pair(eone::PAGE_SCRIPT,     "Script..."),
	std::make_pair(eone::PAGE_BLUEPRINT,  "Blueprint"),

	std::make_pair(eone::PAGE_QUAKE,      "Quake"),
};

}

namespace eone
{

WxFrame::WxFrame()
	: ee0::WxFrame("EasyOne", true)
{
	moon::Blackboard::Instance()->SetWindow(this);
}

void WxFrame::OnNew(wxCommandEvent& event)
{
	ee0::WxListSelectDlg dlg(this, "New page", PAGE_LIST, wxGetMousePosition());
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	auto app = std::static_pointer_cast<Application>(m_app);
	auto type = dlg.GetSelectedID();
#ifndef GAME_OBJ_ECS
	StagePageFactory::Create(type, app->GetStagePanel());
#else
	StagePageFactory::Create(app->GetWorld(), type, app->GetStagePanel());
#endif // GAME_OBJ_ECS

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

}