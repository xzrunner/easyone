#include "scene2d/WxStagePage.h"

namespace eone
{
namespace scene2d
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library)
	: ee2::WxStagePage(parent, library)
{
}

void WxStagePage::Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
	                       const ee0::VariantSet& variants) const
{
	auto var = variants.GetVariant("preview");
	if (var.m_type == ee0::VT_EMPTY) {
		ee2::WxStagePage::Traverse(func);
	} else {
		ee2::WxStagePage::Traverse(func);
	}
}

}
}