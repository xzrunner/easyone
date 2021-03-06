#include "pbrgraph/WxPreviewPanel.h"

#ifdef MODULE_PBRGRAPH

#include <ee0/SubjectMgr.h>
#include <ee3/WorldTravelOP.h>
#include <renderlab/Node.h>
#include <renderlab/Evaluator.h>

#include <painting0/Shader.h>
#include <painting3/PerspCam.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <rendergraph/DrawList.h>
#include <rendergraph/RenderContext.h>
#include <renderpipeline/RenderMgr.h>
#include <renderpipeline/IRenderer.h>

namespace eone
{
namespace pbrgraph
{

WxPreviewPanel::WxPreviewPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
                               const ee0::RenderContext* rc, const std::shared_ptr<renderlab::Evaluator>& eval)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 300))
    , m_sub_mgr(sub_mgr)
    , m_edit_impl(this, m_sub_mgr)
{
    m_canvas = std::make_unique<Canvas>(this, rc, eval);

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

WxPreviewPanel::Canvas::Canvas(WxPreviewPanel* panel,
                               const ee0::RenderContext* rc,
                               const std::shared_ptr<renderlab::Evaluator>& eval)
    : ee0::WxStageCanvas(panel, panel->m_edit_impl, std::make_shared<pt3::PerspCam>(sm::vec3(0, 0, -1.5f), sm::vec3(0, 0, 0), sm::vec3(0, 1, 0)), rc, nullptr, HAS_3D)
    , m_panel(panel)
    , m_eval(eval)
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

void WxPreviewPanel::Canvas::OnDrawSprites() const
{
    if (!m_eval || m_eval->IsEmpty()) {
        return;
    }

    rp::RenderMgr::Instance()->SetRenderer(rp::RenderType::EXTERN);

    auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();

    rendergraph::RenderContext rc(ur_rc);
    rc.cam_proj_mat = m_camera->GetProjectionMat();
    rc.cam_view_mat = m_camera->GetViewMat();
    if (m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>()) {
        auto persp = std::static_pointer_cast<pt3::PerspCam>(m_camera);
        rc.cam_position = persp->GetPos();
    }
    rc.light_position.Set(0, 2, -4);

    m_eval->Draw(rc);
}

}
}

#endif // MODULE_PBRGRAPH