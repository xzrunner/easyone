#include "anim/Utility.h"
#include "anim/Layer.h"
#include "anim/KeyFrame.h"

#include <algorithm>

namespace anim
{

int Utility::GetMaxFrame(const std::vector<LayerPtr>& layers)
{
	int max_frame = -1;
	for (auto& layer : layers)
	{
		auto& frames = layer->GetAllKeyFrames();
		if (!frames.empty()) {
			max_frame = std::max(max_frame, frames.back()->GetFrameIdx());
		}
	}
	return max_frame;
}

}