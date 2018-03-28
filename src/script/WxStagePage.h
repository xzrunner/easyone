#pragma once

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }
namespace n0 { class NodeSharedComp; }

namespace eone
{

class WxStagePage;

namespace script
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node);

	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_SCRIPT; }

protected:
	virtual const n0::NodeSharedComp& GetEditedNodeComp() const override;

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const;

}; // WxStagePage

}
}