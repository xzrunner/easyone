#pragma once

#include "frame/config.h"

#ifdef MODULE_PBRGRAPH

#include <ee0/WxStageCanvas.h>
#include <ee0/Observer.h>
#include <ee0/typedef.h>
#include <ee0/EditPanelImpl.h>
#include <renderlab/RenderGraph.h>

#include <painting3/Viewport.h>
#include <rendergraph/typedef.h>

#include <wx/panel.h>

namespace bp { class Node; }
namespace renderlab { class Evaluator; }

namespace eone
{
namespace pbrgraph
{

class WxPreviewPanel : public wxPanel
{
public:
    WxPreviewPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
        const ee0::RenderContext* rc, const std::shared_ptr<renderlab::Evaluator>& eval);

private:
    void OnSize(wxSizeEvent& event);

private:
    class Canvas : public ee0::WxStageCanvas, public ee0::Observer
    {
    public:
        Canvas(WxPreviewPanel* panel, const ee0::RenderContext* rc,
            const std::shared_ptr<renderlab::Evaluator>& eval);
        virtual ~Canvas();

        virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

        const pt3::Viewport& GetViewport() const { return m_viewport; }

    private:
        virtual void OnDrawSprites() const override;

    private:
        WxPreviewPanel* m_panel;

        pt3::Viewport m_viewport;

        const std::shared_ptr<renderlab::Evaluator>& m_eval;

    }; // Canvas

private:
    ee0::SubjectMgrPtr      m_sub_mgr = nullptr;
    ee0::EditPanelImpl      m_edit_impl;
    std::unique_ptr<Canvas> m_canvas = nullptr;

}; // WxPreviewPanel

}
}

#endif // MODULE_PBRGRAPH