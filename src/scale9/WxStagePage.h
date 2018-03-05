#pragma once

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }

namespace eone
{
namespace scale9
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const n0::SceneNodePtr& node);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const n0::SceneNodePtr&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet()) const override;

	virtual int GetPageType() const override { return PAGE_SCALE9; }

protected:
	virtual const n0::NodeSharedComp& GetEditedNodeComp() const override;

	virtual void LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val);

private:
	void InsertSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();

	void StagePageOnShow();
	
private:
	// 6 7 8
	// 3 4 5
	// 0 1 2
	n0::SceneNodePtr m_grids[9];

}; // WxStagePage

}
}