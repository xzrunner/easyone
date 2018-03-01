#include "frame/WxStagePanel.h"

#include <ee0/WxStagePage.h>

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

ee0::WxStagePage* WxStagePanel::GetCurrentStagePage() const
{
	return dynamic_cast<ee0::WxStagePage*>(GetCurrentPage());
}

void WxStagePanel::AddNewPage(ee0::WxStagePage* page, const std::string& title)
{
	AddPage(page, title);
	SetSelection(GetPageCount() - 1);
}

void WxStagePanel::OnPageChanging(wxAuiNotebookEvent& event)
{
	auto page = GetCurrentStagePage();
	if (page) {
		m_old_page = page;
	}
}

void WxStagePanel::OnPageChanged(wxAuiNotebookEvent& event)
{
	if (!m_old_page) {
		return;
	}

	auto new_page = GetCurrentStagePage();
	GD_ASSERT(new_page, "null new_page");

	ee0::VariantSet vars;

	ee0::Variant var;
	var.m_type = ee0::VT_PVOID;
	var.m_val.pv = new_page;
	vars.SetVariant("new_page", var);

	m_old_page->GetSubjectMgr().NotifyObservers(ee0::MSG_STAGE_PAGE_CHANGING, vars);

	m_old_page = nullptr;
}

}