#pragma once

#include <node0/typedef.h>

#include <wx/dialog.h>
#include <wx/aui/framemanager.h>

namespace ee0 { class RenderContext; }
namespace ee2 { class WxStagePage; }
namespace n2 { class CompMask; }

namespace eone
{
namespace mask
{

class WxEditDialog : public wxDialog
{
public:
	WxEditDialog(wxWindow* parent, const std::shared_ptr<ee0::RenderContext>& rc,
		n0::SceneNodePtr& node, n2::CompMask& cmask);
	virtual ~WxEditDialog();

private:
	void InitLayout(const std::shared_ptr<ee0::RenderContext>& rc);
	void InitNodes(n0::SceneNodePtr& node, n2::CompMask& cmask);

	wxWindow* CreateStagePanel(const std::shared_ptr<ee0::RenderContext>& rc);
	wxWindow* CreatePreviewPanel(const std::shared_ptr<ee0::RenderContext>& rc);
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();

private:
	wxAuiManager m_mgr;

	ee2::WxStagePage* m_preview;
	ee2::WxStagePage* m_stage;

}; // WxEditDialog

}
}