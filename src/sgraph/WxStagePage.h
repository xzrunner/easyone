#pragma once

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

#include <blueprint/Node.h>

namespace eone
{
namespace sgraph
{

class WxToolbarPanel;

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_SHADER_GRAPH; }

	static const std::string PAGE_TYPE;

protected:
	virtual void OnPageInit() override;

	// todo ecs
#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const override;
	virtual void LoadFromFileImpl(const std::string& filepath) override;

private:
	bool InsertSceneObj(const ee0::VariantSet& variants);
	bool DeleteSceneObj(const ee0::VariantSet& variants);
	bool ClearSceneObj();

	bool SetModelType(const std::string& model);

	bool CalcMaterial();

private:
	WxToolbarPanel* m_toolbar = nullptr;

	std::string m_model_type;

	std::shared_ptr<bp::node::Node> m_mat_node = nullptr;

}; // WxStagePage

}
}