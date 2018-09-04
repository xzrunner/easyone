#pragma once

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

#include <ee0/GameObj.h>

#include <anim/Layer.h>

#include <vector>

namespace ee0 { class WxLibraryPanel; }
namespace model { class ModelInstance; class SkeletalAnim; }

namespace eone
{
namespace anim3
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_ANIM3; }

protected:
	virtual void OnPageInit() override;

#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

	virtual void LoadFromFileExt(const std::string& filepath) override;

private:
	void InitTimeLinePanel();
	void InitPropertyPanel();

	bool OnSetCurrFrame(const ee0::VariantSet& variants);
	bool OnRefreshAnimComp();

	void ReloadAnimation(::model::ModelInstance& model_inst,
		::model::SkeletalAnim& sk_anim, int anim_idx);

private:
	std::vector<::anim::LayerPtr> m_layers;

}; // WxStagePage

}
}