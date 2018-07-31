#include "quake/QuakeMapLoader.h"

#include <ee0/MsgHelper.h>

#include <model/Model.h>
#include <model/MapLoader.h>
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompTransform.h>
#include <node3/CompAABB.h>

namespace eone
{
namespace quake
{

void QuakeMapLoader::LoadFromFile(ee0::SubjectMgr& sub_mgr,
	                              const std::string& filepath)
{
	std::vector<std::shared_ptr<model::Model>> models;
	model::MapLoader::Load(models, filepath);

	// insert models
	for (auto& model : models)
	{
		auto node = std::make_shared<n0::SceneNode>();

		// model
		auto& cmodel = node->AddSharedComp<n3::CompModel>();
		cmodel.SetModel(model);
		cmodel.SetFilepath(filepath);
		node->AddUniqueComp<n3::CompModelInst>(model, 0);

		// transform
		node->AddUniqueComp<n3::CompTransform>();

		// aabb
		node->AddUniqueComp<n3::CompAABB>(pt3::AABB(model->aabb));

		// id
		node->AddUniqueComp<n0::CompIdentity>();

		ee0::MsgHelper::InsertNode(sub_mgr, node);
	}
}

}
}