#pragma once

#include <ee0/GameObj.h>

namespace anim { class KeyFrame; class Layer; }
namespace n2 { class CompAnim; class CompAnimInst; }
namespace ee0 { class SubjectMgr; }

namespace eone
{
namespace anim
{

class AnimHelper
{
public:
	static ::anim::Layer* GetLayer(const n2::CompAnim& canim, int layer_idx);
	static ::anim::KeyFrame* GetKeyFrame(const n2::CompAnim& canim, int layer_idx, int frame_idx);

	static int GetMaxFrame(const n2::CompAnim& canim);
	static int GetMaxFrame(const n2::CompAnim& canim, int layer_idx);

	static int GetCurrFrame(const n2::CompAnim& canim, 
		const n2::CompAnimInst& canim_inst);

	static void UpdateTreePanael(ee0::SubjectMgr& sub_mgr,
		const n2::CompAnimInst& canim_inst);

}; // AnimHelper

}
}