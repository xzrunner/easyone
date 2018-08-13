#pragma once

#include <wx/panel.h>

namespace eone
{
namespace model
{

class WxStagePage;

class WxToolbarPanel : public wxPanel
{
public:
	WxToolbarPanel(wxWindow* parent, WxStagePage* stage);

private:
	void InitLayout();

	void OnChangeEditType(wxCommandEvent& event);

private:
	WxStagePage* m_stage;

	wxRadioBox* m_edit_op;

}; // WxToolbarPanel

}
}