#pragma once

#include "frame/config.h"

#ifdef MODULE_SHAPE3D

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

#include <ee0/GameObj.h>
#include <ee3/CameraMgr.h>

#include <draft3/EditOpMgr.h>

namespace ee0 { class WxLibraryPanel; }
namespace pt3 { class Viewport; }

namespace eone
{
namespace shape3d
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_SHAPE3D; }

	void InitEditOP(const std::shared_ptr<pt0::Camera>& cam, const pt3::Viewport& vp);

	void InitViewports();

	auto& GetEditOpMgr() { return m_editor_mgr; }

	static const std::string PAGE_TYPE;

protected:
	virtual void OnPageInit() override;

#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

private:
	void SwitchToNextViewport();

	void InsertSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();

private:
	draft3::EditOpMgr m_editor_mgr;

	ee3::CameraMgr m_cam_mgr;

}; // WxStagePage

}
}

#endif // MODULE_SHAPE3D