#include "frame/WxStagePanel.h"
#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"

#include <guard/check.h>

namespace eone
{

WxStagePanel::WxStagePanel(wxWindow* parent)
	: wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
		wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER)
	, m_old_page(nullptr)
{
	Connect(GetId(), wxEVT_AUINOTEBOOK_PAGE_CHANGING, 
		wxAuiNotebookEventHandler(WxStagePanel::OnPageChanging));
	Connect(GetId(), wxEVT_AUINOTEBOOK_PAGE_CHANGED,
		wxAuiNotebookEventHandler(WxStagePanel::OnPageChanged));
}

WxStagePage* WxStagePanel::GetCurrentStagePage() const
{
	return static_cast<WxStagePage*>(GetCurrentPage());
}

void WxStagePanel::AddNewPage(WxStagePage* page, const std::string& title)
{
	AddPage(page, title);
	SetSelection(GetPageCount() - 1);
}

bool WxStagePanel::SwitchToPage(const std::string& filepath)
{
	for (int i = 0; i < GetPageCount(); ++i)
	{
		auto page = static_cast<WxStagePage*>(GetPage(i));
		if (page->GetFilepath() == filepath) {
			SetSelection(i);
			return true;
		}
	}
	return false;
}

void WxStagePanel::OnPageChanging(wxAuiNotebookEvent& event)
{
	auto page = GetCurrentStagePage();
	if (page) 
	{
		m_old_page = page;
		page->GetSubjectMgr().NotifyObservers(ee0::MSG_STAGE_PAGE_CHANGING);
	}
}

void WxStagePanel::OnPageChanged(wxAuiNotebookEvent& event)
{
	if (!m_old_page) {
		return;
	}

	auto new_page = GetCurrentStagePage();
	GD_ASSERT(new_page, "null new_page");
	Blackboard::Instance()->GetFrame()->SetTitle(new_page->GetFilepath());

	ee0::VariantSet vars;

	ee0::Variant var;
	var.m_type = ee0::VT_PVOID;
	var.m_val.pv = new_page;
	vars.SetVariant("new_page", var);

	m_old_page->GetSubjectMgr().NotifyObservers(ee0::MSG_STAGE_PAGE_CHANGED, vars);

	new_page->GetSubjectMgr().NotifyObservers(ee0::MSG_STAGE_PAGE_ON_SHOW);

	m_old_page = nullptr;
}

}