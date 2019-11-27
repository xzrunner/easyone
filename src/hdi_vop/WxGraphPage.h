#pragma once

#include "frame/config.h"

#ifdef MODULE_HDI_VOP

#include <vopview/WxGraphPage.h>

namespace vopv { class WxToolbarPanel; }

namespace eone
{
namespace hdi_vop
{

class WxGraphPage : public vopv::WxGraphPage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

private:
    void InitToolbarPanel();

private:
    vopv::WxToolbarPanel* m_toolbar = nullptr;

}; // WxGraphPage

}
}

#endif // MODULE_HDI_VOP