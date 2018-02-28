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
	WxEditDialog(wxWindow* parent, const std::shared_ptr<ee0::RenderContext>& rc,
		n0::SceneNodePtr& node, n2::CompScale9* cscale9 = nullptr);
	virtual ~WxEditDialog();

private:
	void InitLayout(const std::shared_ptr<ee0::RenderContext>& rc, n2::CompScale9* cscale9);

	wxWindow* CreateStagePanel(const std::shared_ptr<ee0::RenderContext>& rc, n2::CompScale9* cscale9);
	wxWindow* CreatePreviewPanel(const std::shared_ptr<ee0::RenderContext>& rc);
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();

private:
	wxAuiManager m_mgr;

	ee0::WxStagePage* m_preview;
	ee0::WxStagePage* m_stage;

}; // WxEditDialog

}
}