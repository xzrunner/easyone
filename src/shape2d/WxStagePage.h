#pragma once

#include "frame/config.h"

#ifdef MODULE_SHAPE2D

#include <ee0/GameObj.h>

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }
namespace n0 { class NodeComp; }

namespace eone
{
namespace shape2d
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_SHAPE2D; }

	static const std::string PAGE_TYPE;

protected:
	// todo ecs
#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

private:
	bool InsertSceneObj(const ee0::VariantSet& variants);
	bool DeleteSceneObj(const ee0::VariantSet& variants);
	bool ClearSceneObj();

}; // WxStagePage

}
}

#endif // MODULE_SHAPE2D