#pragma once

#include <wx/aui/auibook.h>

namespace ee0 { class WxStagePage; }

namespace eone
{

class WxStagePage;

class WxStagePanel : public wxAuiNotebook
{
public:
	WxStagePanel(wxWindow* parent);

	ee0::WxStagePage* GetCurrentStagePage() const;

	void AddNewPage(ee0::WxStagePage* page, const std::string& title);
	
private:
	void OnPageChanging(wxAuiNotebookEvent& event);
	void OnPageChanged(wxAuiNotebookEvent& event);

private:
	ee0::WxStagePage* m_old_page;

}; // WxStagePanel

}