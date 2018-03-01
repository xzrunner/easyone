#pragma once

#include <node0/typedef.h>

#include <wx/dialog.h>
#include <wx/aui/framemanager.h>

namespace ee0 { class RenderContext; class WindowContext; }
namespace ee2 { class WxStagePage; }
namespace n2 { class CompMask; }

namespace eone
{
namespace mask
{

class WxEditDialog : public wxDialog
{
public:
	WxEditDialog(wxWindow* parent, const ee0::RenderContext& rc, 
		const ee0::WindowContext& wc, n0::SceneNodePtr& node, n2::CompMask& cmask);
	virtual ~WxEditDialog();

private:
	void InitLayout();
	void InitNodes(n0::SceneNodePtr& node, n2::CompMask& cmask);

	wxWindow* CreateStagePanel();
	wxWindow* CreatePreviewPanel();
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();

private:
	const ee0::RenderContext& m_rc;
	const ee0::WindowContext& m_wc;

	wxAuiManager m_mgr;

	ee2::WxStagePage* m_preview;
	ee2::WxStagePage* m_stage;

}; // WxEditDialog

}
}