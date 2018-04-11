#pragma once

#include <ee0/GameObj.h>
#include <ee0/WxStagePage.h>

#include <rapidjson/document.h>

namespace n0 { class NodeSharedComp; }
namespace ee0 { class VariantSet; }

namespace eone
{


class WxStagePage : public ee0::WxStagePage
{
public:
	enum LayoutType
	{
		LAYOUT_PREVIEW = 0,
		LAYOUT_STAGE_EXT,
		LAYOUT_TOOLBAR,
	};

public:
	enum TraverseType
	{
		TRAV_DRAW_PREVIEW = 1000,
	};

public:
#ifndef GAME_OBJ_ECS
	WxStagePage(wxWindow* parent, const ee0::GameObj& obj, LayoutType layout_type);
#else
	WxStagePage(wxWindow* parent, const ecsx::Entity& entity, LayoutType layout_type);
#endif // GAME_OBJ_ECS

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual int GetPageType() const = 0;

	void StoreToJson(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const;
	void LoadFromJson(const std::string& dir, const rapidjson::Value& val);

	const ee0::GameObj& GetEditedObj() const { return m_obj; }

	void SetFilepath(const std::string& filepath) { m_filepath = filepath; }
	const std::string& GetFilepath() const { return m_filepath; }

protected:
	virtual void OnPageInit() {}

	virtual const n0::NodeSharedComp& GetEditedObjComp() const = 0;

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const {}
	virtual void LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val) {}

private:
	void SetEditorDirty(const ee0::VariantSet& variants);

	void RegisterAllMessages();
	void UnregisterAllMessages();

	void InitSubWindow();

protected:
	ee0::GameObj m_obj;

	LayoutType m_layout_type;

	std::string m_filepath;

	std::vector<uint32_t> m_messages;

}; // WxStagePage

}