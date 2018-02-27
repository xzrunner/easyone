#include "scale9/WxEditDialog.h"
#include "scale9/WxStagePage.h"
#include "scale9/WxStageCanvas.h"

#include "frame/WxSceneTreePanel.h"
#include "frame/WxDetailPanel.h"
#include "frame/NodeSelectOP.h"

#include <ee0/MsgHelper.h>
#include <ee2/WxStagePage.h>
#include <ee2/WxStageCanvas.h>
#include <ee2/CamControlOP.h>

#include <node2/CompMask.h>

namespace eone
{
namespace scale9
{

WxEditDialog::WxEditDialog(wxWindow* parent, const std::shared_ptr<wxGLContext>& glctx,
	                       n0::SceneNodePtr& node, n2::CompScale9& cscale9)
	: wxDialog(parent, wxID_ANY, "Edit Scale9", wxDefaultPosition, 
		wxSize(800, 600), wxCLOSE_BOX | wxCAPTION | wxMAXIMIZE_BOX)
	, m_mgr(this)
{
	InitLayout(glctx, cscale9);
}

WxEditDialog::~WxEditDialog()
{
	m_mgr.UnInit();
}

void WxEditDialog::InitLayout(const std::shared_ptr<wxGLContext>& glctx, n2::CompScale9& cscale9)
{
	m_mgr.AddPane(CreateStagePanel(glctx, cscale9),
		wxAuiPaneInfo().Name("Stage").Caption("Stage").
		Center().PaneBorder(false));

	m_mgr.AddPane(CreatePreviewPanel(glctx),
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

wxWindow* WxEditDialog::CreateStagePanel(const std::shared_ptr<wxGLContext>& glctx, n2::CompScale9& cscale9)
{
	m_stage = new scale9::WxStagePage(this, nullptr, cscale9);
	auto canvas = std::make_shared<ee2::WxStageCanvas>(m_stage, glctx);
	m_stage->GetImpl().SetCanvas(canvas);
	m_stage->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*m_stage));
	return m_stage;
}

wxWindow* WxEditDialog::CreatePreviewPanel(const std::shared_ptr<wxGLContext>& glctx)
{
	m_preview = new ee2::WxStagePage(this, nullptr);
	auto canvas = std::make_shared<WxStageCanvas>(m_preview, glctx);
	m_preview->GetImpl().SetCanvas(canvas);
	auto op = std::make_shared<ee2::CamControlOP>(*canvas->GetCamera(), m_preview->GetSubjectMgr());
	m_preview->GetImpl().SetEditOP(op);
	return m_preview;
}

wxWindow* WxEditDialog::CreateTreePanel()
{
	auto& sub_mgr = m_stage->GetSubjectMgr();
	return new WxSceneTreePanel(this, sub_mgr);
}

wxWindow* WxEditDialog::CreateDetailPanel()
{
	auto& sub_mgr = m_stage->GetSubjectMgr();
	return new WxDetailPanel(this, sub_mgr);
}

}
}