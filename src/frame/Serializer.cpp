#include "frame/Serializer.h"

#include <ee0/WxStagePage.h>
#include <ee0/MsgHelper.h>

#include <node0/SerializeSystem.h>
#include <node0/SceneNode.h>
#include <node2/CompBoundingBox.h>
#include <node2/CompTransform.h>
#include <js/RapidJsonHelper.h>
#include <guard/check.h>

#include <boost/filesystem.hpp>

namespace eone
{

void Serializer::StoreToFile(const ee0::WxStagePage& page, const std::string& filepath)
{
	rapidjson::Document doc;
	doc.SetObject();

	rapidjson::Value nodes_val;
	nodes_val.SetArray();

	auto dir = boost::filesystem::path(filepath).parent_path().string();

	auto& alloc = doc.GetAllocator();
	page.Traverse([&](const n0::SceneNodePtr& node)->bool 
	{
		rapidjson::Value val_node;
		n0::SerializeSystem::StoreNodeToJson(node, dir, val_node, alloc);
		nodes_val.PushBack(val_node, alloc);
		return true;
	});
	doc.AddMember("nodes", nodes_val, alloc);

	js::RapidJsonHelper::WriteToFile(filepath.c_str(), doc);
}

void Serializer::LoadFromFile(ee0::WxStagePage& page, const std::string& filepath)
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

	auto dir = boost::filesystem::path(filepath).parent_path().string();

	auto& nodes_val = doc["nodes"];
	for (auto& node_val : nodes_val.GetArray())
	{
		auto node = std::make_shared<n0::SceneNode>();
		n0::SerializeSystem::LoadNodeFromJson(node, dir, node_val);

		if (node->HasComponent<n2::CompBoundingBox>() && 
			node->HasComponent<n2::CompTransform>()) 
		{
			auto& cbb = node->GetComponent<n2::CompBoundingBox>();
			auto& ctrans = node->GetComponent<n2::CompTransform>();
			cbb.Build(ctrans.GetTrans().GetSRT());
		}

		bool succ = ee0::MsgHelper::InsertNode(page.GetSubjectMgr(), node);
		GD_ASSERT(succ, "no MSG_INSERT_SCENE_NODE");
	}
}

}