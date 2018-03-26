#pragma once

namespace n2 { class CompAnim; }

namespace eone
{
namespace anim
{

class LayerHelper
{
public:
	static int GetMaxFrame(const n2::CompAnim& canim, int layer_idx);
	static int GetMaxFrame(const n2::CompAnim& canim);

}; // LayerHelper

}
}