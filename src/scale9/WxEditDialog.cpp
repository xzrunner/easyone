#include "scale9/WxEditDialog.h"
#include "scale9/WxStagePage.h"
#include "scale9/WxStageCanvas.h"

#include "frame/WxSceneTreePanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/NodeSelectOP.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"

#include <ee0/MsgHelper.h>
#include <ee2/WxStageCanvas.h>
#include <ee2/CamControlOP.h>

#include <node2/CompMask.h>

namespace eone
{
namespace scale9
{

WxEditDialog::WxEditDialog(wxWindow* parent, const ee0::RenderContext& rc,
	                       const ee0::WindowContext& wc, const n0::SceneNodePtr& node)
	: wxDialog(parent, wxID_ANY, "Edit Scale9", wxDefaultPosition, wxSize(800, 600), wxCLOSE_BOX | wxCAPTION | wxMAXIMIZE_BOX)
	, m_rc(rc)
	, m_wc(wc)
	, m_mgr(this)
{
	InitLayout(node);
}

WxEditDialog::~WxEditDialog()
{
	m_mgr.UnInit();
}

void WxEditDialog::InitLayout(const n0::SceneNodePtr& node)
{
	m_mgr.AddPane(CreateStagePanel(node),
		wxAuiPaneInfo().Name("Stage").Caption("Stage").
		Center().PaneBorder(false));

	m_mgr.AddPane(CreatePreviewPanel(),
		wxAuiPaneInfo().Name("Preview").Caption("Preview").
		Center().PaneBorder(false));

	m_mgr.AddPane(CreateTreePanel(),
		wxAuiPaneInfo().Name("Tree").Caption("Tree").
		Right().Row(1).MinSize(100, 0).PaneBorder(false));

	m_mgr.AddPane(CreateDetailPanel(),
		wxAuiPaneInfo().Name("Detail").Caption("Detail").
		Right().MinSize(200, 0).PaneBorder(false));

	m_mgr.Update();
}

wxWindow* WxEditDialog::CreateStagePanel(const n0::SceneNodePtr& node)
{
	m_stage = new scale9::WxStagePage(this, nullptr, node);
	auto canvas = std::make_shared<ee2::WxStageCanvas>(m_stage, &m_rc, &m_wc);
	m_stage->GetImpl().SetCanvas(canvas);
	m_stage->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*m_stage, m_rc, m_wc));
	return m_stage;
}

wxWindow* WxEditDialog::CreatePreviewPanel()
{
	auto& sub_mgr = m_stage->GetSubjectMgr();
	m_preview = new WxPreviewPanel(this, sub_mgr, m_stage);

	auto canvas = std::make_shared<WxPreviewCanvas>(m_preview, m_rc);
	m_preview->GetImpl().SetCanvas(canvas);
	auto op = std::make_shared<ee2::CamControlOP>(*canvas->GetCamera(), sub_mgr);
	m_preview->GetImpl().SetEditOP(op);

	return m_preview;
}

wxWindow* WxEditDialog::CreateTreePanel()
{
	return new WxSceneTreePanel(
		this, m_stage->GetSubjectMgr(), m_stage->GetEditedNode());
}

wxWindow* WxEditDialog::CreateDetailPanel()
{
	return new WxDetailPanel(
		this, m_stage->GetSubjectMgr(), m_stage->GetEditedNode(), m_stage->GetLuaVM());
}

}
}