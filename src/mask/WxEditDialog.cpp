#include "mask/WxEditDialog.h"
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
namespace mask
{

WxEditDialog::WxEditDialog(wxWindow* parent, const std::shared_ptr<ee0::RenderContext>& rc,
	                       n0::SceneNodePtr& node, n2::CompMask& cmask)
	: wxDialog(parent, wxID_ANY, "Edit Mask", wxDefaultPosition, 
		wxSize(800, 600), wxCLOSE_BOX | wxCAPTION | wxMAXIMIZE_BOX)
	, m_mgr(this)
{
	InitLayout(rc);
	InitNodes(node, cmask);
}

WxEditDialog::~WxEditDialog()
{
	m_mgr.UnInit();
}

void WxEditDialog::InitLayout(const std::shared_ptr<ee0::RenderContext>& rc)
{
	m_mgr.AddPane(CreateStagePanel(rc),
		wxAuiPaneInfo().Name("Stage").Caption("Stage").
		Center().PaneBorder(false));

	m_mgr.AddPane(CreatePreviewPanel(rc),
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

void WxEditDialog::InitNodes(n0::SceneNodePtr& node, n2::CompMask& cmask)
{
	// preview
	if (node) {
		ee0::MsgHelper::InsertNode(m_preview->GetSubjectMgr(), node);
	}

	// stage
	if (auto& node = cmask.GetBaseNode()) {
		ee0::MsgHelper::InsertNode(m_stage->GetSubjectMgr(), 
			std::const_pointer_cast<n0::SceneNode>(node));
	}
	if (auto& node = cmask.GetMaskNode()) {
		ee0::MsgHelper::InsertNode(m_stage->GetSubjectMgr(),
			std::const_pointer_cast<n0::SceneNode>(node));
	}
}

wxWindow* WxEditDialog::CreateStagePanel(const std::shared_ptr<ee0::RenderContext>& rc)
{
	m_stage = new ee2::WxStagePage(this, nullptr);
	auto canvas = std::make_shared<ee2::WxStageCanvas>(m_stage, rc);
	m_stage->GetImpl().SetCanvas(canvas);
	m_stage->GetImpl().SetEditOP(std::make_shared<NodeSelectOP>(*m_stage));
	return m_stage;
}

wxWindow* WxEditDialog::CreatePreviewPanel(const std::shared_ptr<ee0::RenderContext>& rc)
{
	m_preview = new ee2::WxStagePage(this, nullptr);
	auto canvas = std::make_shared<ee2::WxStageCanvas>(m_preview, rc);
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