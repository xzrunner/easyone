#pragma once

#include <ee2/WxStagePage.h>

namespace eone
{
namespace scene2d
{

class WxStagePage : public ee2::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library);

	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet()) const override;

}; // WxStagePage

}
}