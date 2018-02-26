#pragma once

#include <ee0/WxStagePage.h>

namespace ee0 { class WxLibraryPanel; }

namespace eone
{
namespace mask
{

class WxStagePage : ee0::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const override;

private:
	void InsertSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();

private:
	n0::SceneNodePtr m_base, m_mask;

}; // WxStagePage

}
}