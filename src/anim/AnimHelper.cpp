#include "anim/AnimHelper.h"

#include "frame/GameObjFactory.h"
#include "frame/MessageID.h"

#include <ee0/SubjectMgr.h>

#include <node0/SceneNode.h>
#include <node0/CompComplex.h>
#include <node2/CompAnim.h>
#include <node2/CompAnimInst.h>
#include <anim/Layer.h>
#include <anim/KeyFrame.h>
#include <anim/Utility.h>

namespace eone
{
namespace anim
{

::anim::Layer* AnimHelper::GetLayer(const n2::CompAnim& canim, int layer_idx)
{
	auto& layers = canim.GetAllLayers();
	if (layer_idx >= 0 && static_cast<size_t>(layer_idx) < layers.size()) {
		return layers[layer_idx].get();
	} else {
		return nullptr;
	}
}

::anim::KeyFrame* AnimHelper::GetKeyFrame(const n2::CompAnim& canim, int layer_idx, int frame_idx)
{
	auto layer = GetLayer(canim, layer_idx);
	if (!layer) {
		return nullptr;
	} else {
		return layer->GetCurrKeyFrame(frame_idx);
	}
}

int AnimHelper::GetMaxFrame(const n2::CompAnim& canim)
{
	return ::anim::Utility::GetMaxFrame(canim.GetAllLayers());
}

int AnimHelper::GetMaxFrame(const n2::CompAnim& canim, int layer_idx)
{
	auto& layers = canim.GetAllLayers();
	if (layer_idx < 0 || static_cast<size_t>(layer_idx) >= layers.size()) {
		return -1;
	}

	auto& layer = layers[layer_idx];
	auto& frames = layer->GetAllKeyFrames();
	if (frames.empty()) {
		return -1;
	} else {
		return frames.back()->GetFrameIdx();
	}
}

int AnimHelper::GetCurrFrame(const n2::CompAnim& canim, 
	                         const n2::CompAnimInst& canim_inst)
{
	int frame_idx = canim_inst.GetPlayCtrl().GetFrame();
	int max_frame = GetMaxFrame(canim);
	if (max_frame > 0) {
		frame_idx = frame_idx % (max_frame + 1);
	}
	return frame_idx;
}

void AnimHelper::UpdateTreePanael(ee0::SubjectMgr& sub_mgr,
	                              const n2::CompAnimInst& canim_inst)
{
	// todo ecs
#ifndef GAME_OBJ_ECS
	auto root = GameObjFactory::Create(GAME_OBJ_COMPLEX2D);
	auto& ccomplex = root->GetSharedComp<n0::CompComplex>();
	canim_inst.TraverseCurrNodes([&](const ee0::GameObj& obj)->bool
	{
		ccomplex.AddChild(obj);
		return true;
	});

	ee0::VariantSet vars;

	ee0::Variant var;
	var.m_type = ee0::VT_PVOID;
	var.m_val.pv = &root;
	vars.SetVariant("obj", var);

	sub_mgr.NotifyObservers(MSG_TREE_PANEL_REBUILD, vars);
#endif // GAME_OBJ_ECS
}

}
}