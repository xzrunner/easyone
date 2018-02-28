#pragma once

#include <node0/typedef.h>

#include <wx/dialog.h>
#include <wx/aui/framemanager.h>

namespace ee0 { class WxStagePage; class RenderContext;  }
namespace n2 { class CompScale9; }

namespace eone
{
namespace scale9
{

class WxEditDialog : public wxDialog
{
public:
	WxEditDialog(wxWindow* parent, const std::shared_ptr<ee0::RenderContext>& edit_rc,
		const std::shared_ptr<ee0::RenderContext>& preview_rc,
		n0::SceneNodePtr& node, n2::CompScale9* cscale9 = nullptr);
	virtual ~WxEditDialog();

private:
	void InitLayout(n2::CompScale9* cscale9);

	wxWindow* CreateStagePanel(n2::CompScale9* cscale9);
	wxWindow* CreatePreviewPanel();
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();

private:
	std::shared_ptr<ee0::RenderContext> m_edit_rc = nullptr;
	std::shared_ptr<ee0::RenderContext> m_preview_rc = nullptr;

	wxAuiManager m_mgr;

	ee0::WxStagePage* m_preview;
	ee0::WxStagePage* m_stage;

}; // WxEditDialog

}
}