#pragma once

#include "frame/config.h"

#ifdef MODULE_PBRGRAPH

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

#include <blueprint/typedef.h>
#include <blueprint/StageFuncNode.h>

namespace renderlab { class Evaluator; }

namespace eone
{
namespace pbrgraph
{

class WxToolbarPanel;

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj);
    virtual ~WxStagePage();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_PBR_GRAPH; }

    auto& GetEval() const { return m_eval; }

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

    void CreateNewPage(const ee0::VariantSet& variants) const;

    bool UpdateNodes();
    void UpdateBlueprint();

    void LoadFunctionNodes();

    auto& GetFuncNodeHelper() { return m_func_node_helper; }

private:
    WxToolbarPanel* m_toolbar = nullptr;

    std::shared_ptr<renderlab::Evaluator> m_eval = nullptr;

    bp::StageFuncNode m_func_node_helper;

}; // WxStagePage

}
}

#endif // MODULE_PBRGRAPH