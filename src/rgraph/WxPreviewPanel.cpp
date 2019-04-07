#include "rgraph/WxPreviewPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WorldTravelOP.h>

#include <painting0/Shader.h>
#include <painting3/PerspCam.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <rendergraph/DrawList.h>
#include <unirender/Blackboard.h>
#include <renderpipeline/RenderMgr.h>
#include <renderpipeline/IRenderer.h>

namespace eone
{
namespace rgraph
{

WxPreviewPanel::WxPreviewPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
                               const ee0::RenderContext* rc)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 300))
    , m_sub_mgr(sub_mgr)
    , m_edit_impl(this, m_sub_mgr)
{
    m_canvas = std::make_unique<Canvas>(this, rc);

    auto op = std::make_shared<ee3::WorldTravelOP>(
        m_canvas->GetCamera(), m_canvas->GetViewport(), m_sub_mgr
        );
    m_edit_impl.SetEditOP(op);

    Bind(wxEVT_SIZE, &WxPreviewPanel::OnSize, this, GetId());
}

void WxPreviewPanel::OnSize(wxSizeEvent& event)
{
    m_canvas->SetSize(event.GetSize());
}

//////////////////////////////////////////////////////////////////////////
// class WxPreviewPanel::Canvas
//////////////////////////////////////////////////////////////////////////

WxPreviewPanel::Canvas::Canvas(WxPreviewPanel* panel, const ee0::RenderContext* rc)
    : ee0::WxStageCanvas(panel, panel->m_edit_impl, std::make_shared<pt3::PerspCam>(sm::vec3(0, 0, -1.5f), sm::vec3(0, 0, 0), sm::vec3(0, 1, 0)), rc, nullptr, HAS_3D)
    , m_panel(panel)
{
    panel->m_sub_mgr->RegisterObserver(ee0::MSG_SET_CANVAS_DIRTY, this);
}

WxPreviewPanel::Canvas::~Canvas()
{
    m_panel->m_sub_mgr->UnregisterObserver(ee0::MSG_SET_CANVAS_DIRTY, this);
}

void WxPreviewPanel::Canvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
    switch (msg)
    {
    case ee0::MSG_SET_CANVAS_DIRTY:
        SetDirty();
        break;
    }
}

void WxPreviewPanel::Canvas::RebuildDrawList(const bp::Node& node)
{
    m_rg.ClearCache();
    m_rg_node = m_rg.CreateGraphNode(node);
    m_draw_list = std::make_shared<rg::DrawList>(m_rg_node);
}

void WxPreviewPanel::Canvas::OnSize(int w, int h)
{
	auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
	if (wc)
	{
		wc->SetScreen(w, h);
		m_viewport.SetSize(w, h);

		m_camera->OnSize(static_cast<float>(w), static_cast<float>(h));
		wc->SetProjection(m_camera->GetProjectionMat());
	}
}

void WxPreviewPanel::Canvas::OnDrawSprites() const
{
    if (m_draw_list)
    {
        rp::RenderMgr::Instance()->SetRenderer(rp::RenderType::EXTERN);

        auto& rc = ur::Blackboard::Instance()->GetRenderContext();
        m_draw_list->Draw(rc);
    }
}

}
}