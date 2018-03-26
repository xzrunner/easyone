#pragma once

#include <node2/CompAnim.h>
//namespace n2 { class CompAnim; }

namespace eone
{
namespace anim
{

class LayerHelper
{
public:
	static int GetMaxFrame(const n2::CompAnim& canim, int layer_idx);
	static int GetMaxFrame(const n2::CompAnim& canim);

	static void InsertNullFrame(n2::CompAnim::Layer& layer, int time);
	static void RemoveNullFrame(n2::CompAnim::Layer& layer, int time);

	static void InsertKeyFrame(n2::CompAnim::Layer& layer, int time);
	static void RemoveKeyFrame(n2::CompAnim::Layer& layer, int time);

	static bool IsKeyFrame(const n2::CompAnim::Layer& layer, int time);

	static n2::CompAnim::Frame* GetCurrFrame(const n2::CompAnim::Layer& layer, int time);

}; // LayerHelper

}
}