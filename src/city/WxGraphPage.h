#pragma once

#include "frame/config.h"

#ifdef MODULE_CITY

#include <cgaview/WxGraphPage.h>

namespace eone
{
namespace city
{

class WxGraphPage : public cgav::WxGraphPage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

}; // WxGraphPage

}
}

#endif // MODULE_CITY