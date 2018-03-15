#pragma once

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

#include <node0/typedef.h>

namespace ee0 { class WxLibraryPanel; }
namespace n0 { class NodeSharedComp; }

namespace eone
{

class WxStagePage;

namespace scene2d
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_SCENE2D; }

protected:
	virtual const n0::NodeSharedComp& GetEditedNodeComp() const override;

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const;

private:
	bool InsertSceneNode(const ee0::VariantSet& variants);
	bool DeleteSceneNode(const ee0::VariantSet& variants);
	bool ClearSceneNode();
	bool ReorderSceneNode(const ee0::VariantSet& variants);

}; // WxStagePage

}
}