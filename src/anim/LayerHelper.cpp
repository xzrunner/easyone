#include "anim/LayerHelper.h"

#include <node2/CompAnim.h>

namespace eone
{
namespace anim
{

int LayerHelper::GetMaxFrame(const n2::CompAnim& canim, int layer_idx)
{
	auto& layers = canim.GetAllLayers();
	if (layer_idx < 0 || static_cast<size_t>(layer_idx) >= layers.size()) {
		return -1;
	}

	auto& layer = layers[layer_idx];
	if (layer->frames.empty()) {
		return -1;
	} else {
		return layer->frames.back()->index;
	}
}

int LayerHelper::GetMaxFrame(const n2::CompAnim& canim)
{
	int max_frame = -1;
	auto& layers = canim.GetAllLayers();
	for (auto& layer : layers) {
		if (!layer->frames.empty()) {
			max_frame = std::max(max_frame, layer->frames.back()->index);
		}
	}
	return max_frame;
}

}
}