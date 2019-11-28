#include "hdi_sop/WxGraphPage.h"

#ifdef MODULE_HDI_SOP

#include "frame/Blackboard.h"
#include "frame/WxStageSubPanel.h"

#include <blueprint/Blueprint.h>
#include <sopview/WxToolbarPanel.h>

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

    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();

    auto toolbar = static_cast<sopv::WxToolbarPanel*>(toolbar_panel->AddPagePanel([&](wxPanel* parent)->wxPanel* {
        return new sopv::WxToolbarPanel(toolbar_panel, this, GetSceneTree());
    }, wxVERTICAL));
    SetToolbarPanel(toolbar);
}

}
}

#endif // MODULE_HDI_SOP