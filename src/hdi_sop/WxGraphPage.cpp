#include "hdi_sop/WxGraphPage.h"

#ifdef MODULE_HDI_SOP

#include "frame/Blackboard.h"
#include "frame/WxStageSubPanel.h"

#include <blueprint/Blueprint.h>
#include <sopview/WxToolbarPanel.h>
#include <sopview/WxStageCanvas.h>
#include <sopview/WxGeoProperty.h>

namespace eone
{
namespace hdi_sop
{

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::GameObj& obj)
    : sopv::WxGraphPage(parent, nullptr, obj)
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
    sopv::WxGraphPage::SetPreviewCanvas(canvas);

    if (canvas)
    {
        auto sop_canvas = std::static_pointer_cast<sopv::WxStageCanvas>(canvas);
        sop_canvas->SetPropView(m_toolbar->GetGeoPropView());
        sop_canvas->SetGraphStage(this);
    }
}

void WxGraphPage::InitToolbarPanel()
{
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();

    assert(!m_toolbar);
    m_toolbar = static_cast<sopv::WxToolbarPanel*>(toolbar_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new sopv::WxToolbarPanel(toolbar_panel, this, GetSceneTree());
    }, wxVERTICAL));
}

}
}

#endif // MODULE_HDI_SOP