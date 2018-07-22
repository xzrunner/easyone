#pragma once

#include <ee0/GameObj.h>
#include <ee0/typedef.h>

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }
namespace pt3 { class Camera; class Viewport; }

namespace eone
{
namespace quake
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_QUAKE; }

	void InitEditOP(pt3::Camera& cam, const pt3::Viewport& vp);

protected:
	virtual void OnPageInit() override;

#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const override;
	virtual void LoadFromFileImpl(const std::string& filepath) override;

private:
	void InsertSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();

private:
	ee0::EditOPPtr m_default_op   = nullptr;
	ee0::EditOPPtr m_rotate_op    = nullptr;
	ee0::EditOPPtr m_translate_op = nullptr;

}; // WxStagePage

}
}