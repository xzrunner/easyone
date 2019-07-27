#pragma once

#include "frame/config.h"

#ifdef MODULE_ITTGRAPH

#include "frame/WxStagePage.h"

namespace itt { class Evaluator; }

namespace eone
{
namespace ittgraph
{

class WxToolbarPanel;

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

    auto& GetEval() const { return m_eval; }

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas) {
        m_preview_canvas = canvas;
    }

    static const std::string PAGE_TYPE;

protected:
    virtual void OnPageInit() override;

    virtual const n0::NodeComp& GetEditedObjComp() const override;

    virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
        rapidjson::MemoryPoolAllocator<>& alloc) const override;
    // move to LoadFromJson()
    //virtual void LoadFromFileExt(const std::string& filepath) override;

private:
    void InitToolbarPanel();

    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);
    bool ClearSceneObj();

    bool AfterInsertNodeConn(const ee0::VariantSet& variants);
    bool BeforeDeleteNodeConn(const ee0::VariantSet& variants);
    bool UpdateNodeProp(const ee0::VariantSet& variants);

//    bool UpdateNodes();

private:
    WxToolbarPanel* m_toolbar = nullptr;

    std::shared_ptr<itt::Evaluator> m_eval = nullptr;

    std::shared_ptr<ee0::WxStageCanvas> m_preview_canvas = nullptr;

}; // WxGraphPage

}
}

#endif // MODULE_ITTGRAPH