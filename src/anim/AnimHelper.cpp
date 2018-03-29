#include "anim/AnimHelper.h"

#include <node2/CompAnim.h>
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

int AnimHelper::GetCurrFrame(const n2::CompAnim& canim)
{
	int frame_idx = canim.GetPlayCtrl().GetFrame(canim.GetFPS());
	int max_frame = GetMaxFrame(canim);
	if (max_frame > 0) {
		frame_idx = frame_idx % (max_frame + 1);
	}
	return frame_idx;
}

}
}