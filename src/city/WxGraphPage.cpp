#include "city/WxGraphPage.h"

#ifdef MODULE_CITY

#include "frame/Blackboard.h"
#include "frame/WxStageSubPanel.h"

#include <blueprint/Blueprint.h>
#include <cgaview/WxToolbarPanel.h>

namespace eone
{
namespace city
{

WxGraphPage::WxGraphPage(wxWindow* parent, const ee0::GameObj& obj)
    : cgav::WxGraphPage(parent, nullptr, obj)
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        bp::Blueprint::Instance();
    }

    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();

    auto toolbar = new cgav::WxToolbarPanel(toolbar_panel, this);
    toolbar_panel->AddPagePanel(toolbar, wxVERTICAL);
    SetToolbarPanel(toolbar);
}

}
}

#endif // MODULE_CITY