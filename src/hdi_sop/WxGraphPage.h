#pragma once

#include "frame/config.h"

#ifdef MODULE_HDI_SOP

#include <sopview/WxGraphPage.h>

namespace eone
{
namespace hdi_sop
{

class WxGraphPage : public sopv::WxGraphPage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

}; // WxGraphPage

}
}

#endif // MODULE_HDI_SOP