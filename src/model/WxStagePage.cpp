#include "model/WxStagePage.h"
#include "model/WxPreviewPanel.h"
#include "model/WxPreviewCanvas.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/AppStyle.h"
#include "frame/WxStageExtPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxStageDropTarget.h>
#include <ee3/EditSkeletonOP.h>
#include <ee3/WorldTravelOP.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <ns/CompFactory.h>

namespace eone
{
namespace model
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_STAGE_EXT | SHOW_DETAIL)
{
	m_messages.push_back(ee0::MSG_INSERT_SCENE_NODE);
	m_messages.push_back(ee0::MSG_DELETE_SCENE_NODE);
	m_messages.push_back(ee0::MSG_CLEAR_SCENE_NODE);

	if (library) {
		SetDropTarget(new ee3::WxStageDropTarget(ECS_WORLD_VAR library, this));
	}
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	//switch (msg)
	//{
	//case ee0::MSG_INSERT_SCENE_NODE:
	//	InsertSceneNode(variants);
	//	break;
	//case ee0::MSG_DELETE_SCENE_NODE:
	//	DeleteSceneNode(variants);
	//	break;
	//case ee0::MSG_CLEAR_SCENE_NODE:
	//	ClearSceneNode();
	//	break;
	//}
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

void WxStagePage::OnPageInit()
{
	auto bb = Blackboard::Instance();

	auto panel = bb->GetStageExtPanel();
	auto sizer = panel->GetSizer();
	if (sizer) {
		sizer->Clear(true);
	}
	else {
		sizer = new wxBoxSizer(wxHORIZONTAL);
	}

	auto preview_panel = new WxPreviewPanel(panel);
	sizer->Add(preview_panel, 1, wxEXPAND);

	auto preview_canvas = std::make_shared<WxPreviewCanvas>(
		preview_panel, bb->GetRenderContext(), m_obj);
	preview_panel->GetImpl().SetCanvas(preview_canvas);

	auto preview_op = std::make_shared<ee3::WorldTravelOP>(
		preview_canvas->GetCamera(), preview_canvas->GetViewport(), preview_panel->GetSubjectMgr());
	preview_panel->GetImpl().SetEditOP(preview_op);

	m_preview_submgr = preview_panel->GetSubjectMgr();

	panel->SetSizer(sizer);
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n3::CompModel>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
	                             rapidjson::MemoryPoolAllocator<>& alloc) const
{
	val.AddMember("is_scene3d", true, alloc);
}

void WxStagePage::LoadFromFileImpl(const std::string& filepath)
{
	auto casset = ns::CompFactory::Instance()->CreateAsset(filepath);
	assert(casset->AssetTypeID() == n0::GetAssetUniqueTypeID<n3::CompModel>());

	auto cmodel = std::static_pointer_cast<n3::CompModel>(casset);
	auto& cmode_inst = m_obj->GetUniqueComp<n3::CompModelInst>();
	cmode_inst.SetModel(cmodel->GetModel(), 0);

	auto op = std::dynamic_pointer_cast<ee3::EditSkeletonOP>(
		GetImpl().GetEditOP()
	);
	op->SetModel(cmode_inst.GetModel().get());
}

void WxStagePage::InsertSceneNode(const ee0::VariantSet& variants)
{
}

void WxStagePage::DeleteSceneNode(const ee0::VariantSet& variants)
{
}

void WxStagePage::ClearSceneNode()
{
}

}
}