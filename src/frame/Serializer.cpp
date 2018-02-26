#include "frame/Serializer.h"

#include <ee0/WxStagePage.h>
#include <ee0/MsgHelper.h>

#include <node0/SerializeSystem.h>
#include <js/RapidJsonHelper.h>
#include <guard/check.h>
#include <ns/Tools.h>

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
		auto node = ns::Tools::CreateNode(dir, node_val);
		bool succ = ee0::MsgHelper::InsertNode(page.GetSubjectMgr(), node);
		GD_ASSERT(succ, "no MSG_INSERT_SCENE_NODE");
	}
}

}