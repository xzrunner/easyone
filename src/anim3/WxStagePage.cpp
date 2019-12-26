#include "anim3/WxStagePage.h"

#ifdef MODULE_ANIM3

#include "anim3/WxPropertyPanel.h"
#include "anim3/CompBone.h"

#include "frame/WxStagePage.h"
#include "frame/Blackboard.h"
#include "frame/Application.h"
#include "frame/typedef.h"
#include "frame/AppStyle.h"
#include "frame/WxStageSubPanel.h"

#include <ee0/SubjectMgr.h>
#include <ee3/WxStageDropTarget.h>
#include <ee3/SkeletonJointOP.h>
#include <ee3/WorldTravelOP.h>

#include <eanim/Callback.h>
#include <eanim/WxTimelinePanel.h>
#include <eanim/MessageID.h>

#include <guard/check.h>
#include <node0/SceneNode.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <anim/PlayCtrl.h>
#include <ns/CompFactory.h>
#include <model/Model.h>
#include <model/SkeletalAnim.h>

namespace eone
{
namespace anim3
{

WxStagePage::WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, ECS_WORLD_PARAM const ee0::GameObj& obj)
	: eone::WxStagePage(parent, ECS_WORLD_VAR obj, SHOW_STAGE | SHOW_STAGE_EXT | SHOW_TOOLBAR)
{
	m_messages.push_back(eanim::MSG_SET_CURR_FRAME);
	m_messages.push_back(eanim::MSG_REFRESH_ANIM_COMP);

	if (library) {
		SetDropTarget(new ee3::WxStageDropTarget(ECS_WORLD_VAR library, this));
	}

	m_layers.push_back(std::make_unique<::anim::Layer>());
}

void WxStagePage::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
	eone::WxStagePage::OnNotify(msg, variants);

	bool dirty = false;
	switch (msg)
	{
	case eanim::MSG_SET_CURR_FRAME:
		dirty = OnSetCurrFrame(variants);
		break;
	case eanim::MSG_REFRESH_ANIM_COMP:
		dirty = OnRefreshAnimComp();
		break;
	}

	if (dirty) {
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
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

void WxStagePage::OnPageInit()
{
	InitTimeLinePanel();
	InitPropertyPanel();
}

#ifndef GAME_OBJ_ECS
const n0::NodeComp& WxStagePage::GetEditedObjComp() const
{
	return m_obj->GetSharedComp<n3::CompModel>();
}
#endif // GAME_OBJ_ECS

void WxStagePage::LoadFromFileExt(const std::string& filepath)
{
	const int ANIM_IDX = 0;

	auto casset = ns::CompFactory::Instance()->CreateAsset(filepath);
	assert(casset->AssetTypeID() == n0::GetAssetUniqueTypeID<n3::CompModel>());

	auto cmodel = std::static_pointer_cast<n3::CompModel>(casset);
	auto& cmode_inst = m_obj->GetUniqueComp<n3::CompModelInst>();
	cmode_inst.SetModel(cmodel->GetModel(), ANIM_IDX);

	auto op = std::dynamic_pointer_cast<ee3::SkeletonJointOP>(
		GetImpl().GetEditOP()
	);
	op->SetModel(cmode_inst.GetModel().get());

	auto& ext = cmodel->GetModel()->ext;
	if (ext && ext->Type() == model::EXT_SKELETAL)
	{
		auto sk_anim = static_cast<model::SkeletalAnim*>(ext.get());
		if (sk_anim) {
			ReloadAnimation(*cmode_inst.GetModel().get(), *sk_anim, ANIM_IDX);
		}
	}
}

void WxStagePage::InitTimeLinePanel()
{
	// set callback
	eanim::Callback::Funs funs;
	funs.refresh_all_nodes = [&]() {
		// todo
	};
	funs.get_all_layers = [&]()->const std::vector<::anim::LayerPtr>& {
		return m_layers;
	};
	funs.add_layer = [&](::anim::LayerPtr& layer) {
		m_layers.push_back(std::move(layer));
	};
	funs.swap_layer = [&](int from, int to) {
		assert(from >= 0 && from < static_cast<int>(m_layers.size())
			&& to >= 0 && to < static_cast<int>(m_layers.size()));
		std::swap(m_layers[from], m_layers[to]);
	};
	funs.remove_all_layers = [&]()->bool {
		m_layers.clear();
		return true;
	};
	eanim::Callback::RegisterCallback(funs);

    assert(!m_timeline);
    static anim::PlayCtrl pctrl;
    auto stage_ext_panel = Blackboard::Instance()->GetStageExtPanel();
    m_timeline = new eanim::WxTimelinePanel(stage_ext_panel, m_sub_mgr, pctrl);
    stage_ext_panel->AddPagePanel(m_timeline, wxVERTICAL);
}

void WxStagePage::InitPropertyPanel()
{
    assert(!m_property);
    auto toolbar_panel = Blackboard::Instance()->GetToolbarPanel();
    m_property = new WxPropertyPanel(toolbar_panel, m_sub_mgr, m_layers);
    toolbar_panel->AddPagePanel(m_property, wxVERTICAL);
}

bool WxStagePage::OnSetCurrFrame(const ee0::VariantSet& variants)
{
	auto var = variants.GetVariant("frame");
	GD_ASSERT(var.m_type == ee0::VT_INT, "err frame");
	int frame = var.m_val.l;

	auto& cmode_inst = m_obj->GetUniqueComp<n3::CompModelInst>();
	auto& model_inst = cmode_inst.GetModel();
	if (!model_inst) {
		return false;
	}
	model_inst->SetFrame(frame * model_inst->GetModel()->anim_speed);

	m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);

	return true;
}

bool WxStagePage::OnRefreshAnimComp()
{
	return false;
}

void WxStagePage::ReloadAnimation(::model::ModelInstance& model_inst,
	                              ::model::SkeletalAnim& sk_anim, int anim_idx)
{
	auto& all_anims = sk_anim.GetAnims();
	assert(anim_idx >= 0 && anim_idx < static_cast<int>(all_anims.size()));
	auto& anim = all_anims[anim_idx];

	float fps = anim->ticks_per_second != 0 ? anim->ticks_per_second : 30;
	float spd = 1.0f / model_inst.GetModel()->anim_speed;

	m_layers.clear();
	auto& layers = anim->channels;
	m_layers.reserve(layers.size());
	int bone_idx = 0;
	for (auto& src_layer : layers)
	{
		auto dst_layer = std::make_unique<anim::Layer>();
		dst_layer->SetName(src_layer->name);

		for (auto& src_rot : src_layer->rotation_keys)
		{
			int frame = src_rot.first * spd;
			auto key_frame = std::make_unique<::anim::KeyFrame>(frame);

			auto node = std::make_shared<n0::SceneNode>();
			auto& cbone = node->AddSharedComp<CompBone>(model_inst, sk_anim, anim_idx, bone_idx, frame);
			cbone.SetRotation(src_rot.second);
			key_frame->AddNode(node);

			dst_layer->AddKeyFrame(key_frame);
		}

		m_layers.push_back(std::move(dst_layer));

		++bone_idx;
	}
}

}
}

#endif // MODULE_ANIM3