#pragma once

#include "frame/config.h"

#ifdef MODULE_MODEL

#include <ee0/GameObj.h>

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace pt0 { class Camera; }
namespace pt3 { class Viewport; }
namespace ee0 { class WxLibraryPanel; }
namespace ee3 { class SkeletonJointOP; class SkeletonIKOP; class MeshIKOP; }

namespace eone
{
namespace model
{

class WxToolbarPanel;

class WxStagePage : public eone::WxStagePage
{
public:
	enum EditOpType
	{
		OP_ROTATE_JOINT = 0,
		OP_TRANSLATE_JOINT,
		OP_SKELETAL_IK,
		OP_MESH_IK,
	};

public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_MODEL; }

	void InitEditOp(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp);
	void SetEditOp(EditOpType type);

protected:
	virtual void OnPageInit() override;

#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

	virtual void LoadFromFileExt(const std::string& filepath) override;

private:
	void InitPreviewPanel();
	void InitToolbarPanel();

private:
	ee0::SubjectMgrPtr m_preview_submgr = nullptr;

	std::shared_ptr<ee3::SkeletonJointOP> m_sk_op      = nullptr;
	std::shared_ptr<ee3::SkeletonIKOP>    m_sk_ik_op   = nullptr;
	std::shared_ptr<ee3::MeshIKOP>        m_mesh_ik_op = nullptr;

    wxPanel* m_preview = nullptr;
	WxToolbarPanel* m_toolbar = nullptr;

	n0::SceneNodePtr m_preview_obj = nullptr;

}; // WxStagePage

}
}

#endif // MODULE_MODEL