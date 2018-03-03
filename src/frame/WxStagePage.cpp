#include "frame/WxStagePage.h"

#include <ns/CompSerializer.h>

namespace eone
{

WxStagePage::WxStagePage(wxWindow* parent, const n0::SceneNodePtr& node)
	: ee0::WxStagePage(parent)
	, m_node(node)
{
}

void WxStagePage::StoreToJson(const std::string& dir, rapidjson::Value& val,
	                          rapidjson::MemoryPoolAllocator<>& alloc) const
{
	ns::CompSerializer::Instance()->ToJson(GetEditedNodeComp(), dir, val, alloc);
	StoreToJsonExt(dir, val, alloc);
}

void WxStagePage::LoadFromJson(const std::string& dir, const rapidjson::Value& val)
{
	ns::CompSerializer::Instance()->FromJson(m_node, dir, val);
	LoadFromJsonExt(dir, val);
}

}