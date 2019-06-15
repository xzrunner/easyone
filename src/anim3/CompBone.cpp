#include "anim3/CompBone.h"

#ifdef MODULE_ANIM3

namespace eone
{
namespace anim3
{

const char* const CompBone::TYPE_NAME = "eone_anim3";

CompBone::CompBone(model::ModelInstance& model_inst,
	               model::SkeletalAnim& sk_anim,
	               int anim_idx, int bone_idx, int frame)
	: m_model_inst(model_inst)
	, m_sk_anim(sk_anim)
	, m_anim_idx(anim_idx)
	, m_bone_idx(bone_idx)
	, m_frame(frame)
{
	m_scaling.Set(1, 1, 1);
}

}
}

#endif // MODULE_ANIM3