#pragma once

#include "frame/config.h"

#ifdef MODULE_PROTOTYPE

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }

namespace eone
{
namespace prototype
{

class WxToolbarPanel;

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_PROTOTYPING; }

	static const std::string PAGE_TYPE;

protected:
	virtual void OnPageInit() override;

	// todo ecs
#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const override;
	virtual void LoadFromFileExt(const std::string& filepath) override;

private:
	bool InsertSceneObj(const ee0::VariantSet& variants);
	bool DeleteSceneObj(const ee0::VariantSet& variants);
	bool ClearSceneObj();

	bool ReloadSceneObj();

	void InitScriptContext();
	void InitLibraryPanel();
	void InitToolbarPanel();

private:
	wxPanel* m_toolbar = nullptr;

}; // WxStagePage

}
}

#endif // MODULE_PROTOTYPE