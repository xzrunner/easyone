#include "hdi_vop/WxGraphPage.h"

#ifdef MODULE_HDI_VOP

#include "frame/Blackboard.h"
#include "frame/WxStageSubPanel.h"

#include <blueprint/Blueprint.h>
#include <vopview/WxToolbarPanel.h>
#include <vopview/WxStageCanvas.h>

namespace eone
{
namespace hdi_vop
{

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::GameObj& obj)
    : vopv::WxGraphPage(parent, nullptr, obj)
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        bp::Blueprint::Instance();
    }

    InitToolbarPanel();
}

void WxGraphPage::SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas)
{
    vopv::WxGraphPage::SetPreviewCanvas(canvas);
    if (canvas)
    {
        auto vop_canvas = std::static_pointer_cast<vopv::WxStageCanvas>(canvas);
        vop_canvas->SetGraphStage(this);
    }
}

void WxGraphPage::InitToolbarPanel()
{
    assert(!m_toolbar);
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();
    m_toolbar = new vopv::WxToolbarPanel(toolbar_panel, this);
    toolbar_panel->AddPagePanel(m_toolbar, wxVERTICAL);
}

}
}

#endif // MODULE_HDI_VOP