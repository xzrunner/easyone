#pragma once

#include <ee0/WxStagePage.h>

namespace ee0 { class WxLibraryPanel; }
namespace n2 { class CompScale9;  }

namespace eone
{
namespace scale9
{

class WxStagePage : public ee0::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library,
		n2::CompScale9* cscale9 = nullptr);
	virtual ~WxStagePage();

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func) const override;

private:
	void InsertSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();

	bool DeleteSceneNode(const n0::SceneNodePtr& node);

private:
	n2::CompScale9* m_cscale9 = nullptr;
	bool m_comp_new;
	
}; // WxStagePage

}
}