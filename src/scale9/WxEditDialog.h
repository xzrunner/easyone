#pragma once

#include <node0/typedef.h>

#include <wx/dialog.h>
#include <wx/aui/framemanager.h>

namespace ee0 { class WxStagePage; class RenderContext; class WindowContext; }
namespace n2 { class CompScale9; }

namespace eone
{
namespace scale9
{

class WxEditDialog : public wxDialog
{
public:
	WxEditDialog(wxWindow* parent, const ee0::RenderContext& rc,
		const ee0::WindowContext& wc, n0::SceneNodePtr& node, n2::CompScale9& cscale9);
	virtual ~WxEditDialog();

private:
	void InitLayout(n2::CompScale9& cscale9);

	wxWindow* CreateStagePanel(n2::CompScale9& cscale9);
	wxWindow* CreatePreviewPanel();
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();

private:
	const ee0::RenderContext& m_rc;
	const ee0::WindowContext& m_wc;

	wxAuiManager m_mgr;

	ee0::WxStagePage* m_preview;
	ee0::WxStagePage* m_stage;

}; // WxEditDialog

}
}