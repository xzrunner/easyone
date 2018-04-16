#pragma once

#include <ee0/GameObj.h>
#include <ee0/Config.h>

#include <wx/dialog.h>
#include <wx/aui/framemanager.h>

namespace ee0 { class WxStagePage; class RenderContext; class WindowContext; }
ECS_WORLD_DECL

namespace eone
{

class WxPreviewPanel;
class WxStagePage;

namespace scale9
{

class WxEditDialog : public wxDialog
{
public:
	WxEditDialog(wxWindow* parent, const ee0::RenderContext& rc,
		const ee0::WindowContext& wc, ECS_WORLD_PARAM const ee0::GameObj& obj);
	virtual ~WxEditDialog();

private:
	void InitLayout(const ee0::GameObj& obj);

	wxWindow* CreateStagePanel(const ee0::GameObj& obj);
	wxWindow* CreatePreviewPanel();
	wxWindow* CreateTreePanel();
	wxWindow* CreateDetailPanel();

private:
	const ee0::RenderContext& m_rc;
	const ee0::WindowContext& m_wc;
	ECS_WORLD_SELF_DEF

	wxAuiManager m_mgr;

	WxPreviewPanel*    m_preview;
	eone::WxStagePage* m_stage;

}; // WxEditDialog

}
}