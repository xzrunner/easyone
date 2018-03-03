#include "frame/Serializer.h"
#include "frame/WxStagePage.h"

#include <ee0/MsgHelper.h>

#include <js/RapidJsonHelper.h>
#include <guard/check.h>
#include <ns/NodeFactory.h>
#include <ns/NodeSerializer.h>

#include <boost/filesystem.hpp>

namespace eone
{

void Serializer::StoreToFile(const WxStagePage& page, const std::string& filepath)
{
	rapidjson::Document doc;
	doc.SetArray();

	auto dir = boost::filesystem::path(filepath).parent_path().string();

	auto& alloc = doc.GetAllocator();

	page.StoreToJson(dir, doc, alloc);

	js::RapidJsonHelper::WriteToFile(filepath.c_str(), doc);
}

void Serializer::LoadFromFile(WxStagePage& page, const std::string& filepath)
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

	auto dir = boost::filesystem::path(filepath).parent_path().string();

	page.LoadFromJson(dir, doc);
}

}