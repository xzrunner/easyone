#pragma once

#include "frame/config.h"

#ifdef MODULE_HDI_SOP

#include "frame/WxStagePage.h"

#include <sopview/WxGraphPage.h>

namespace bp { class Node; }
namespace sopv { class SceneTree; class WxToolbarPanel; }

namespace eone
{
namespace hdi_sop
{

class WxGraphPage : public eone::WxStagePage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

    virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
        const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

    virtual int GetPageType() const { return 0; }

    auto GetSceneTree() const { return m_sop_page.GetSceneTree(); }

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas);

    void SetRootNode(const n0::SceneNodePtr& root) { m_obj = root; }

    static const std::string PAGE_TYPE;

protected:
    virtual void OnPageInit() override;

    virtual const n0::NodeComp& GetEditedObjComp() const override;

private:
    void InitToolbarPanel();

private:
    sopv::WxGraphPage m_sop_page;

    sopv::WxToolbarPanel* m_toolbar = nullptr;

}; // WxGraphPage

}
}

#endif // MODULE_HDI_SOP