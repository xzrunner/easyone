#include "frame/Serializer.h"

#include <ee0/WxStagePage.h>
#include <ee0/MsgHelper.h>

#include <js/RapidJsonHelper.h>
#include <guard/check.h>
#include <ns/NodeFactory.h>
#include <ns/NodeSerializer.h>

#include <boost/filesystem.hpp>

namespace eone
{

void Serializer::StoreToFile(const ee0::WxStagePage& page, const std::string& filepath)
{
	rapidjson::Document doc;
	doc.SetArray();

	auto dir = boost::filesystem::path(filepath).parent_path().string();

	auto& alloc = doc.GetAllocator();
	page.Traverse([&](const n0::SceneNodePtr& node)->bool 
	{
		rapidjson::Value val_node;
		ns::NodeSerializer::StoreNodeToJson(node, dir, val_node, alloc);
		doc.PushBack(val_node, alloc);
		return true;
	});

	js::RapidJsonHelper::WriteToFile(filepath.c_str(), doc);
}

void Serializer::LoadFromFile(ee0::WxStagePage& page, const std::string& filepath)
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

	auto dir = boost::filesystem::path(filepath).parent_path().string();
	for (auto& node_val : doc.GetArray())
	{
		auto node = ns::NodeFactory::CreateNode(dir, node_val);
		bool succ = ee0::MsgHelper::InsertNode(page.GetSubjectMgr(), node);
		GD_ASSERT(succ, "no MSG_INSERT_SCENE_NODE");
	}
}

}