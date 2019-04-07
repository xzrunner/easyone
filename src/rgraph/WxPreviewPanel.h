#pragma once

#include <ee0/WxStageCanvas.h>
#include <ee0/Observer.h>
#include <ee0/typedef.h>
#include <ee0/EditPanelImpl.h>
#include <renderlab/RenderGraph.h>

#include <painting3/Viewport.h>
#include <rendergraph/typedef.h>

#include <wx/panel.h>

namespace rg { class DrawList; }
namespace bp { class Node; }

namespace eone
{
namespace rgraph
{

class WxPreviewPanel : public wxPanel
{
public:
    WxPreviewPanel(wxWindow* parent, const ee0::SubjectMgrPtr& sub_mgr,
        const ee0::RenderContext* rc);

    void RebuildDrawList(const bp::Node& node) {
        m_canvas->RebuildDrawList(node);
    }

private:
    void OnSize(wxSizeEvent& event);

private:
    class Canvas : public ee0::WxStageCanvas, public ee0::Observer
    {
    public:
        Canvas(WxPreviewPanel* panel, const ee0::RenderContext* rc);
        virtual ~Canvas();

        virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

        const pt3::Viewport& GetViewport() const { return m_viewport; }

        void RebuildDrawList(const bp::Node& node);

    private:
        virtual void OnSize(int w, int h) override;
        virtual void OnDrawSprites() const override;

    private:
        WxPreviewPanel* m_panel;

        pt3::Viewport m_viewport;

        rlab::RenderGraph m_rg;

        rg::NodePtr m_rg_node = nullptr;
        std::shared_ptr<rg::DrawList> m_draw_list = nullptr;

    }; // Canvas

private:
    ee0::SubjectMgrPtr      m_sub_mgr = nullptr;
    ee0::EditPanelImpl      m_edit_impl;
    std::unique_ptr<Canvas> m_canvas = nullptr;

}; // WxPreviewPanel

}
}