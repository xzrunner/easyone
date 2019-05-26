#pragma once

#include "frame/WxStagePage.h"

namespace eone
{
namespace guigraph
{

class WxGraphPage : public WxStagePage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override
	{}

}; // WxGraphPage

}
}