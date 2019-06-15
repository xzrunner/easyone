#pragma once

#include "frame/config.h"

#ifdef MODULE_MODEL

#include <ee0/typedef.h>

#include <wx/panel.h>
#include <wx/scrolwin.h>

class wxNotebook;

namespace ee0 { class WxImageVList; }
namespace ee3 { class WxSkeletalTreeCtrl; }
namespace model { struct Model; }

namespace eone
{
namespace model
{

class WxStagePage;

class WxToolbarPanel : public wxPanel
{
public:
	WxToolbarPanel(wxWindow* parent, WxStagePage* stage);

	void LoadModel(const ::model::Model& model);

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

	wxPanel*           m_edit_page;
	WxTreeScrolled*    m_tree_page;
	ee0::WxImageVList* m_texture_page;

}; // WxToolbarPanel

}
}

#endif // MODULE_MODEL