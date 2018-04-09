#pragma once

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

namespace ee0 { class WxLibraryPanel; }

namespace eone
{
namespace mask
{

class WxStagePage : public eone::WxStagePage
{
public:
	WxStagePage(wxWindow* parent, ee0::WxLibraryPanel* library, const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
		const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

	virtual int GetPageType() const override { return PAGE_MASK; }

protected:
	virtual const n0::NodeSharedComp& GetEditedObjComp() const override;

private:
	void InsertSceneNode(const ee0::VariantSet& variants);
	void DeleteSceneNode(const ee0::VariantSet& variants);
	void ClearSceneNode();

}; // WxStagePage

}
}