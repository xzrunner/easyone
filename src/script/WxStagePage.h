#pragma once

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }
namespace n0 { class NodeComp; }
ECS_WORLD_DECL

namespace eone
{
namespace script
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library,
		ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_SCRIPT; }

protected:
#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const;

}; // WxStagePage

}
}