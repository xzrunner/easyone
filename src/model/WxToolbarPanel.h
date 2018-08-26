#pragma once

#include <ee0/typedef.h>

#include <wx/panel.h>
#include <wx/scrolwin.h>

class wxNotebook;

namespace ee3 { class WxSkeletalTreeCtrl; }

namespace eone
{
namespace model
{

class WxStagePage;

class WxToolbarPanel : public wxPanel
{
public:
	WxToolbarPanel(wxWindow* parent, WxStagePage* stage);

	auto GetTreePanel() { return m_tree->m_tree; }

private:
	void InitLayout();

	void OnChangeEditType(wxCommandEvent& event);

private:
	class WxTreeScrolled : public wxScrolledWindow
	{
	public:
		WxTreeScrolled(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr);

	public:
		ee3::WxSkeletalTreeCtrl* m_tree;

	}; // WxTreeScrolled

private:
	WxStagePage* m_stage;

	wxNotebook* m_notebook;

	WxTreeScrolled* m_tree;
	wxRadioBox*     m_edit;

}; // WxToolbarPanel

}
}