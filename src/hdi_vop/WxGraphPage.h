#pragma once

#include "frame/config.h"

#ifdef MODULE_HDI_VOP

#include "frame/WxStagePage.h"

namespace bp { class Node; }
namespace vopv { class SceneTree; class WxToolbarPanel; class Evaluator; }

namespace eone
{
namespace hdi_vop
{

class WxGraphPage : public eone::WxStagePage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
        const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

    virtual int GetPageType() const { return 0; }

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

    void SetRootNode(const n0::SceneNodePtr& root) { m_obj = root; }

    auto GetEval() const { return m_eval; }

    static const std::string PAGE_TYPE;

protected:
    virtual void OnPageInit() override;

    virtual const n0::NodeComp& GetEditedObjComp() const override;

private:
    void InitToolbarPanel();

    bool ClearAllSceneObjs();
    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);

private:
    vopv::WxToolbarPanel* m_toolbar = nullptr;

    std::shared_ptr<ee0::WxStageCanvas> m_preview_canvas = nullptr;

    std::shared_ptr<vopv::Evaluator> m_eval = nullptr;

}; // WxGraphPage

}
}

#endif // MODULE_HDI_VOP