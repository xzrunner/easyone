#pragma once

#include <node0/typedef.h>

#include <wx/dialog.h>
#include <wx/aui/framemanager.h>

namespace ee0 { class RenderContext; class WindowContext; class WxStagePage; }

namespace eone
{
namespace mask
{

class WxEditDialog : public wxDialog
{
public:
	WxEditDialog(wxWindow* parent, const ee0::RenderContext& rc, 
		const ee0::WindowContext& wc, const n0::SceneNodePtr& node);
	virtual ~WxEditDialog();

private:
	void InitLayout(const n0::SceneNodePtr& node);

	wxWindow* CreateStagePanel(const n0::SceneNodePtr& node);
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