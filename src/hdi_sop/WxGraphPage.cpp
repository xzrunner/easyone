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

    auto toolbar = new sopv::WxToolbarPanel(toolbar_panel, this, GetSceneTree());
    toolbar_panel->AddPagePanel(toolbar, wxVERTICAL);
    SetToolbarPanel(toolbar);
}

}
}

#endif // MODULE_HDI_SOP