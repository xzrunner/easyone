#pragma once

#include <wx/aui/auibook.h>

namespace ee0 { class WxStagePage; }

namespace eone
{

class WxStagePanel : public wxAuiNotebook
{
public:
	WxStagePanel(wxWindow* parent);

	ee0::WxStagePage* GetCurrentStagePage() const;

private:
	void OnPageChanging(wxAuiNotebookEvent& event);
	void OnPageChanged(wxAuiNotebookEvent& event);

private:
	ee0::WxStagePage* m_last_page;

}; // WxStagePanel

}