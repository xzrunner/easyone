#include "model/WxStagePage.h"
#include "model/WxPreviewPanel.h"
#include "model/WxPreviewCanvas.h"
#include "model/WxToolbarPanel.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/AppStyle.h"
#include "frame/WxStageSubPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxStageDropTarget.h>
#include <ee3/SkeletonJointOP.h>
#include <ee3/SkeletonIKOP.h>
#include <ee3/MeshIKOP.h>
#include <ee3/WorldTravelOP.h>
#include <ee3/MessageID.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <ns/CompFactory.h>
#include <model/Model.h>

namespace eone
{
namespace model
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_STAGE_EXT | STAGE_EXT_LFET | SHOW_TOOLBAR)
{
	m_messages.push_back(ee0::MSG_SET_CANVAS_DIRTY);
	m_messages.push_back(ee3::MSG_SKELETAL_TREE_ON_SELECT);

	if (library) {
		SetDropTarget(new ee3::WxStageDropTarget(ECS_WORLD_VAR library, this));
	}
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_SET_CANVAS_DIRTY:
		m_preview_submgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
		break;
	case ee3::MSG_SKELETAL_TREE_ON_SELECT:
		{
			ee3::SkeletonSelectOp* op = static_cast<ee3::SkeletonSelectOp*>(GetImpl().GetEditOP().get());
			op->SetSelected(variants.GetVariant("joint_id").m_val.l);
		}
		break;
	}
}

void WxStagePage::Traverse(std::function<bool(const ee0::GameObj&)> func,
	                       const ee0::VariantSet& variants,
	                       bool inverse) const
{
	auto var = variants.GetVariant("type");
	if (var.m_type == ee0::VT_EMPTY)
	{
		// todo ecs
#ifndef GAME_OBJ_ECS
		m_obj->GetSharedComp<n3::CompModel>().Traverse(func, inverse);
#endif // GAME_OBJ_ECS
		return;
	}

	GD_ASSERT(var.m_type == ee0::VT_LONG, "err type");
	switch (var.m_val.l)
	{
	case TRAV_DRAW_PREVIEW:
		//func(m_obj);
		break;
		// todo ecs
#ifndef GAME_OBJ_ECS
	default:
//		m_obj->GetSharedComp<n3::CompModel>().Traverse(func, inverse);
		func(m_obj);
#endif // GAME_OBJ_ECS
	}
}

void WxStagePage::InitEditOp(const std::shared_ptr<pt0::Camera>& camera,
	                         const pt3::Viewport& vp)
{
	auto prev_op = std::make_shared<ee3::WorldTravelOP>(camera, vp, m_sub_mgr);

	m_sk_op = std::make_shared<ee3::SkeletonJointOP>(camera, vp, m_sub_mgr);
	m_sk_op->SetPrevEditOP(prev_op);

	m_sk_ik_op = std::make_shared<ee3::SkeletonIKOP>(camera, vp, m_sub_mgr);
	m_sk_ik_op->SetPrevEditOP(prev_op);

	m_mesh_ik_op = std::make_shared<ee3::MeshIKOP>(camera, GetImpl().GetCanvas(), vp);
	m_mesh_ik_op->SetPrevEditOP(prev_op);

	GetImpl().SetEditOP(m_sk_op);
}

void WxStagePage::SetEditOp(EditOpType type)
{
	switch (type)
	{
	case OP_ROTATE_JOINT:
		GetImpl().SetEditOP(m_sk_op);
		m_sk_op->ChangeToOpRotate();
		break;
	case OP_TRANSLATE_JOINT:
		GetImpl().SetEditOP(m_sk_op);
		m_sk_op->ChangeToOpTranslate();
		break;
	case OP_SKELETAL_IK:
		GetImpl().SetEditOP(m_sk_ik_op);
		break;
	case OP_MESH_IK:
		GetImpl().SetEditOP(m_mesh_ik_op);
		break;
	}
}

void WxStagePage::OnPageInit()
{
	InitPreviewPanel();
	InitToolbarPanel();
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n3::CompModel>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::LoadFromFileExt(const std::string& filepath)
{
	auto casset = ns::CompFactory::Instance()->CreateAsset(filepath);
	assert(casset->AssetTypeID() == n0::GetAssetUniqueTypeID<n3::CompModel>());

	auto cmodel = std::static_pointer_cast<n3::CompModel>(casset);
    auto& model = cmodel->GetModel();

	auto& cmode_inst = m_obj->GetUniqueComp<n3::CompModelInst>();
	cmode_inst.SetModel(model, 0);

    m_preview_obj->GetSharedComp<n3::CompModel>().SetModel(model);
    m_preview_obj->GetUniqueComp<n3::CompModelInst>().SetModel(model, 0);

	auto model_inst = cmode_inst.GetModel().get();
	m_sk_op->SetModel(model_inst);
	m_sk_ik_op->SetModel(model_inst);
	m_mesh_ik_op->SetModel(model);

	m_toolbar->LoadModel(*model);
}

void WxStagePage::InitPreviewPanel()
{
    auto stage_ext_panel = Blackboard::Instance()->GetStageExtPanel();
    auto preview_panel = static_cast<WxPreviewPanel*>(stage_ext_panel->SetPagePanel(PAGE_MODEL, [](wxPanel* parent)->wxPanel* {
        return new WxPreviewPanel(parent);
    }, wxHORIZONTAL));

	m_preview_obj = m_obj->Clone();
	auto preview_canvas = std::make_shared<WxPreviewCanvas>(
		preview_panel, Blackboard::Instance()->GetRenderContext(), m_preview_obj);
	preview_panel->GetImpl().SetCanvas(preview_canvas);

	auto preview_op = std::make_shared<ee3::WorldTravelOP>(
		preview_canvas->GetCamera(), preview_canvas->GetViewport(), preview_panel->GetSubjectMgr());
	preview_panel->GetImpl().SetEditOP(preview_op);

	m_preview_submgr = preview_panel->GetSubjectMgr();
}

void WxStagePage::InitToolbarPanel()
{
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();
    m_toolbar = static_cast<WxToolbarPanel*>(toolbar_panel->SetPagePanel(PAGE_MODEL, [&](wxPanel* parent)->wxPanel* {
        return new WxToolbarPanel(parent, this);
    }, wxVERTICAL));
}

}
}