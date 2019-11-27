#pragma once

#include "frame/config.h"

#ifdef MODULE_HDI_SOP

#include <sopview/WxGraphPage.h>

namespace sopv { class WxToolbarPanel; }

namespace eone
{
namespace hdi_sop
{

class WxGraphPage : public sopv::WxGraphPage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

private:
    void InitToolbarPanel();

private:
    sopv::WxToolbarPanel* m_toolbar = nullptr;

}; // WxGraphPage

}
}

#endif // MODULE_HDI_SOP