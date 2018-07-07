#pragma once

#include "frame/EditBackup.h"

#include <ee0/GameObj.h>
#include <ee0/WxStagePage.h>

#include <rapidjson/document.h>

namespace n0 { class NodeComp; }
namespace ee0 { class VariantSet; }
ECS_WORLD_DECL

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
	WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj, LayoutType layout_type);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual int GetPageType() const = 0;

	void StoreToJson(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const;
	void LoadFromFile(const std::string& filepath);

	const ee0::GameObj& GetEditedObj() const { return m_obj; }

	void SetFilepath(const std::string& filepath);
	const std::string& GetFilepath() const { return m_filepath; }

protected:
	virtual void OnPageInit() {}

	// todo ecs
#ifndef GAME_OBJ_ECS
	virtual const n0::NodeComp& GetEditedObjComp() const = 0;
#endif // GAME_OBJ_ECS

	virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const {}
	virtual void LoadFromFileImpl(const std::string& filepath) {}

private:
	std::string GetBackupPath() const;
	void LoadFromBackup();

	void SetEditorDirty(const ee0::VariantSet& variants);

	void RegisterAllMessages();
	void UnregisterAllMessages();

	void InitSubWindow();

	void SetMoonContext();

	void ResetNextID();

protected:
	ECS_WORLD_SELF_DEF
	ee0::GameObj m_obj;

	LayoutType m_layout_type;

	std::string m_filepath;

	std::vector<uint32_t> m_messages;

	EditBackup m_backup;

}; // WxStagePage

}