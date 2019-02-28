#include "mask/WxEditDialog.h"

#include "frame/WxWorldPanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/WxPreviewPanel.h"
#include "frame/WxPreviewCanvas.h"
#include "frame/PanelFactory.h"

#include "mask/WxStagePage.h"

#include <ee0/MsgHelper.h>
#include <ee2/WxStageCanvas.h>
#include <ee2/CamControlOP.h>
#include <ee2/NodeSelectOP.h>

#include <node2/CompMask.h>

namespace eone
{
namespace mask
{

WxEditDialog::WxEditDialog(wxWindow* parent, const ee0::RenderContext& rc,
	                       const ee0::WindowContext& wc, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: wxDialog(parent, wxID_ANY, "Edit Mask", wxDefaultPosition, wxSize(800, 600), wxCLOSE_BOX | wxCAPTION | wxMAXIMIZE_BOX)
	, m_rc(rc)
	, m_wc(wc)
	ECS_WORLD_SELF_ASSIGN
	, m_mgr(this)
{
	InitLayout(obj);
}

WxEditDialog::~WxEditDialog()
{
	m_mgr.UnInit();
}

void WxEditDialog::InitLayout(const ee0::GameObj& obj)
{
	m_mgr.AddPane(CreateStagePanel(obj),
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

wxWindow* WxEditDialog::CreateStagePanel(const ee0::GameObj& obj)
{
	m_stage = new mask::WxStagePage(this, nullptr, ECS_WORLD_SELF_VAR obj);

	auto canvas = std::make_shared<ee2::WxStageCanvas>(m_stage, ECS_WORLD_SELF_VAR &m_rc, &m_wc);
	m_stage->GetImpl().SetCanvas(canvas);

    auto op = PanelFactory::CreateNode2DSelectOP(canvas->GetCamera(), *m_stage, m_rc, m_wc);
    m_stage->GetImpl().SetEditOP(op);

	return m_stage;
}

wxWindow* WxEditDialog::CreatePreviewPanel()
{
	auto& sub_mgr = m_stage->GetSubjectMgr();
	m_preview = new WxPreviewPanel(this, sub_mgr, m_stage);

	auto canvas = std::make_shared<WxPreviewCanvas>(m_preview, ECS_WORLD_SELF_VAR m_rc);
	m_preview->GetImpl().SetCanvas(canvas);
	auto op = std::make_shared<ee2::CamControlOP>(canvas->GetCamera(), sub_mgr);
	m_preview->GetImpl().SetEditOP(op);

	return m_preview;
}

wxWindow* WxEditDialog::CreateTreePanel()
{
	return new WxWorldPanel(
		this, m_stage->GetSubjectMgr(), ECS_WORLD_SELF_VAR m_stage->GetEditedObj());
}

wxWindow* WxEditDialog::CreateDetailPanel()
{
	return new WxDetailPanel(
		this, m_stage->GetSubjectMgr(), ECS_WORLD_SELF_VAR m_stage->GetEditedObj(), m_stage->GetMoonCtx());
}

}
}