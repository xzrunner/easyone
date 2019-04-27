#pragma once

#include <ee0/GameObj.h>

#include <uniphysics/cloth/JobManager.h>

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }
namespace up
{
namespace cloth
{
    class Factory;
    class Cloth;
    class MultithreadedSolverHelper;
}
}

namespace eone
{
namespace physics3d
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_PHYSICS3D; }

    virtual void OnSetSkybox(const std::string& filepath);

    void DoSimulationStep(float dt);

	static const std::string PAGE_TYPE;

protected:
    virtual void OnPageInit() override;

#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const override;

private:
	void InsertSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();

    void InitClothObj();

private:
    up::cloth::JobManager m_job_mgr;

    std::unique_ptr<up::cloth::Factory> m_factory = nullptr;

    std::map<std::shared_ptr<up::cloth::Solver>, up::cloth::MultithreadedSolverHelper> m_solvers;

}; // WxStagePage

}
}