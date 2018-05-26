#pragma once

#include <ee0/GameObj.h>
#include <ee0/Config.h>

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }
namespace n0 { class NodeComp; }
ECS_WORLD_DECL

namespace eone
{
namespace anim
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_ANIM; }

protected:
	virtual void OnPageInit() override;

#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

	virtual void LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val);

private:
	bool OnSetCurrFrame(const ee0::VariantSet& variants);
	bool OnRefreshAnimComp();

}; // WxStagePage

}
}