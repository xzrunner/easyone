#pragma once

#include "frame/WxStagePage.h"

namespace eone
{
namespace guigraph
{

class WxGraphPage : public eone::WxStagePage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
        const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

    virtual int GetPageType() const { return 0; }

    // fixme: copy from eone::WxStagePage::LoadFromFile()
    void LoadFromJson(const rapidjson::Value& val, const std::string& dir);

    static const std::string PAGE_TYPE;

protected:
    virtual const n0::NodeComp& GetEditedObjComp() const override;

    virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
        rapidjson::MemoryPoolAllocator<>& alloc) const override;
    virtual void LoadFromFileExt(const std::string& filepath) override;

private:
    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);
    bool ClearSceneObj();

    void UpdateBlueprint();

    bool UpdateNodes();

}; // WxGraphPage

}
}