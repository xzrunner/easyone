#pragma once

#include <ee0/WxStagePage.h>

#include <node0/typedef.h>

#include <rapidjson/document.h>

namespace n0 { class NodeSharedComp; }
namespace ee0 { class VariantSet; }

namespace eone
{


class WxStagePage : public ee0::WxStagePage
{
public:
	enum SubWndType
	{
		SUB_WND_PREVIEW = 0,
		SUB_WND_STAGE_EXT,
	};

public:
	enum TraverseType
	{
		TRAV_DRAW_PREVIEW = 1000,
	};

public:
	WxStagePage(wxWindow* parent, const n0::SceneNodePtr& node, SubWndType sub_wnd_type);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual int GetPageType() const = 0;

	void StoreToJson(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const;
	void LoadFromJson(const std::string& dir, const rapidjson::Value& val);

	const n0::SceneNodePtr& GetEditedNode() const { return m_node; }

	void SetFilepath(const std::string& filepath) { m_filepath = filepath; }
	const std::string& GetFilepath() const { return m_filepath; }

protected:
	virtual void OnPageInit() {}

	virtual const n0::NodeSharedComp& GetEditedNodeComp() const = 0;

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const {}
	virtual void LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val) {}

private:
	void SetEditorDirty(const ee0::VariantSet& variants);

	void RegisterAllMessages();
	void UnregisterAllMessages();

	void InitSubWindow();

protected:
	n0::SceneNodePtr m_node;

	SubWndType m_sub_wnd_type;

	std::string m_filepath;

	std::vector<uint32_t> m_messages;

}; // WxStagePage

}