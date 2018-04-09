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
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_SCALE9; }

protected:
	virtual void OnPageInit() override;

	virtual const n0::NodeSharedComp& GetEditedObjComp() const override;

	virtual void LoadFromJsonExt(const std::string& dir, const rapidjson::Value& val);

private:
	void InsertSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();
	
	void TraverseGrids(std::function<bool(const ee0::GameObj&)> func) const;

private:
	// 6 7 8
	// 3 4 5
	// 0 1 2
	ee0::GameObj m_grids[9];

}; // WxStagePage

}
}