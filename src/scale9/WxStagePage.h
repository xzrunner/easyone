#pragma once

#include "frame/WxStagePage.h"

namespace ee0 { class WxLibraryPanel; }

namespace eone
{
namespace scale9
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet()) const override;

protected:
	virtual const n0::NodeComponent& GetEditedNodeComp() const override;

private:
	void InsertSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();

	bool DeleteSceneNode(const n0::SceneNodePtr& node);
	
}; // WxStagePage

}
}