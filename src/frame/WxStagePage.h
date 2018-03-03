#pragma once

#include <ee0/WxStagePage.h>

#include <node0/typedef.h>

#include <rapidjson/document.h>

namespace n0 { class NodeComponent; }

namespace eone
{

class WxStagePage : public ee0::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, const n0::SceneNodePtr& node);

	virtual int GetPageType() const = 0;

	void StoreToJson(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const;
	void LoadFromJson(const std::string& dir, const rapidjson::Value& val);

	void SetFilepath(const std::string& filepath) { m_filepath = filepath; }
	const std::string& GetFilepath() const { return m_filepath; }

protected:
	virtual const n0::NodeComponent& GetEditedNodeComp() const = 0;

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const {}
	virtual void LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val) {}

protected:
	n0::SceneNodePtr m_node;

	std::string m_filepath;

}; // WxStagePage

}