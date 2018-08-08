#include "frame/GameObjType.h"

#ifndef GAME_OBJ_ECS
#include <node0/SceneNode.h>
#include <node0/CompAsset.h>
#include <node0/CompComplex.h>
#include <node2/CompImage.h>
#include <node2/CompText.h>
#include <node2/CompMask.h>
#include <node2/CompMesh.h>
#include <node2/CompScale9.h>
#include <node2/CompAnim.h>
#include <node2/CompParticle3d.h>
#include <node3/CompModel.h>
#endif // GAME_OBJ_ECS

namespace eone
{

GameObjType GetObjType(const ee0::GameObj& obj)
{
	auto asset_type = obj->GetSharedComp<n0::CompAsset>().AssetTypeID();
	if (asset_type == n0::GetAssetUniqueTypeID<n2::CompImage>()) {
		return GAME_OBJ_IMAGE;
	} else if (asset_type == n0::GetAssetUniqueTypeID<n2::CompText>()) {
		return GAME_OBJ_TEXT;
	} else if (asset_type == n0::GetAssetUniqueTypeID<n2::CompMask>()) {
		return GAME_OBJ_MASK;
	} else if (asset_type == n0::GetAssetUniqueTypeID<n2::CompMesh>()) {
		return GAME_OBJ_MESH;
	} else if (asset_type == n0::GetAssetUniqueTypeID<n2::CompScale9>()) {
		return GAME_OBJ_SCALE9;
	} else if (asset_type == n0::GetAssetUniqueTypeID<n2::CompAnim>()) {
		return GAME_OBJ_ANIM;
	} else if (asset_type == n0::GetAssetUniqueTypeID<n2::CompParticle3d>()) {
		return GAME_OBJ_PARTICLE3D;
	} else if (asset_type == n0::GetAssetUniqueTypeID<n3::CompModel>()) {
		return GAME_OBJ_MODEL;
	} else if (asset_type == n0::GetAssetUniqueTypeID<n0::CompComplex>()) {
		return GAME_OBJ_COMPLEX;
	} else {
		return GAME_OBJ_UNKNOWN;
	}
}

}