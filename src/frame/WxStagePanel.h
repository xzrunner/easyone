#pragma once

#include <wx/aui/auibook.h>

namespace eone
{

class WxStagePage;

class WxStagePanel : public wxAuiNotebook
{
public:
	WxStagePanel(wxWindow* parent);

	WxStagePage* GetCurrentStagePage() const;

	void AddNewPage(WxStagePage* page, const std::string& title);
	
private:
	void OnPageChanging(wxAuiNotebookEvent& event);
	void OnPageChanged(wxAuiNotebookEvent& event);

private:
	WxStagePage* m_old_page;

}; // WxStagePanel

}