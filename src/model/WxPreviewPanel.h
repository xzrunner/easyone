#pragma once

#include "frame/config.h"

#ifdef MODULE_MODEL

#include <ee0/WxStagePage.h>

namespace eone
{
namespace model
{

class WxPreviewPanel : public ee0::WxStagePage
{
public:
	WxPreviewPanel(wxWindow* parent);

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override
	{}

}; // WxPreviewPanel

}
}

#endif // MODULE_MODEL