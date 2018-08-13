#pragma once

#include <SM_Vector.h>
#include <SM_Quaternion.h>
#include <node0/CompAsset.h>

namespace model { class ModelInstance; class SkeletalAnim; }

namespace eone
{
namespace anim3
{

class CompBone : public n0::CompAsset
{
public:
	CompBone(model::ModelInstance& model_inst, model::SkeletalAnim& sk_anim,
		int anim_idx, int bone_idx, int frame);

	virtual const char* Type() const override { return TYPE_NAME; }

	virtual n0::AssetID AssetTypeID() const override {
		return n0::GetAssetUniqueTypeID<CompBone>();
	}
	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
		bool inverse = false) const override {}

	void SetPosition(const sm::vec3& position) {
		m_position = position;
	}
	void SetRotation(const sm::Quaternion& rotation) {
		m_rotation = rotation;
	}
	void SetScaling(const sm::vec3& scaling) {
		m_scaling = scaling;
	}

	auto& GetPosition() const { return m_position; }
	auto& GetRotation() const { return m_rotation; }
	auto& GetScaling() const  { return m_scaling; }

	static const char* const TYPE_NAME;

private:
	model::ModelInstance& m_model_inst;
	model::SkeletalAnim&  m_sk_anim;

	int m_anim_idx = -1;
	int m_bone_idx = -1;

	int m_frame = 0;

	// local transform
	sm::vec3       m_position;
	sm::Quaternion m_rotation;
	sm::vec3       m_scaling;

}; // CompBone

}
}
