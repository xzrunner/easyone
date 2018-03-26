#include "anim/LayerHelper.h"

#include <node2/CompAnim.h>

namespace
{

struct FrameLessThan
{
	bool operator() (const n2::CompAnim::FramePtr& left, const n2::CompAnim::FramePtr& right)
	{
		return left->index < right->index;
	}
	bool operator() (const n2::CompAnim::FramePtr& left, int right)
	{
		return left->index < right;
	}
	bool operator() (int left, const n2::CompAnim::FramePtr& right)
	{
		return left < right->index;
	}
}; // FrameLessThan

}

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

void LayerHelper::InsertNullFrame(n2::CompAnim::Layer& layer, int time)
{
	auto itr = std::lower_bound(layer.frames.begin(), layer.frames.end(), 
		time, FrameLessThan());
	for ( ; itr != layer.frames.end(); ++itr) {
		++(*itr)->index;
	}
}

void LayerHelper::RemoveNullFrame(n2::CompAnim::Layer& layer, int time)
{
	auto itr = std::lower_bound(layer.frames.begin(), layer.frames.end(), 
		time, FrameLessThan());
	for ( ; itr != layer.frames.end(); ++itr) {
		--(*itr)->index;
	}
}

void LayerHelper::InsertKeyFrame(n2::CompAnim::Layer& layer, int time)
{
	auto frame = std::make_unique<n2::CompAnim::Frame>();
	frame->index = time;
	layer.frames.insert(std::upper_bound(layer.frames.begin(), layer.frames.end(), 
		frame, FrameLessThan()), std::move(frame));
}

void LayerHelper::RemoveKeyFrame(n2::CompAnim::Layer& layer, int time)
{
	auto itr = std::lower_bound(layer.frames.begin(), layer.frames.end(),
		time, FrameLessThan());
	layer.frames.erase(itr);
}

bool LayerHelper::IsKeyFrame(const n2::CompAnim::Layer& layer, int time)
{
	auto itr = std::lower_bound(layer.frames.begin(), layer.frames.end(),
		time, FrameLessThan());
	return itr != layer.frames.end() && (*itr)->index == time;
}

n2::CompAnim::Frame* LayerHelper::GetCurrFrame(const n2::CompAnim::Layer& layer, int time)
{
	auto itr = std::lower_bound(layer.frames.begin(), layer.frames.end(),
		time, FrameLessThan());
	if (itr != layer.frames.end()) {
		return itr->get();
	} else {
		return nullptr;
	}
}

}
}