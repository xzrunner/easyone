#pragma once

#include "frame/config.h"

#ifdef MODULE_ITTGRAPH

#include "frame/WxStagePage.h"

namespace bp { class Node; }
namespace itt { class SceneTree; class WxToolbarPanel; }

namespace eone
{
namespace ittgraph
{

class WxGraphPage : public eone::WxStagePage
{
public:
	WxGraphPage(wxWindow* parent, const ee0::GameObj& obj);

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
        const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

    virtual int GetPageType() const { return 0; }

    auto GetSceneTree() const { return m_stree; }

    void SetPreviewCanvas(const std::shared_ptr<ee0::WxStageCanvas>& canvas) {
        m_preview_canvas = canvas;
    }

    void SetRootNode(const n0::SceneNodePtr& root) { m_obj = root; }

    static const std::string PAGE_TYPE;

protected:
    virtual void OnPageInit() override;

    virtual const n0::NodeComp& GetEditedObjComp() const override;

private:
    void InitToolbarPanel();

    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);

    bool AfterInsertNodeConn(const ee0::VariantSet& variants);
    bool BeforeDeleteNodeConn(const ee0::VariantSet& variants);
    bool UpdateNodeProp(const ee0::VariantSet& variants);

    bool ChangeSceneRoot(const ee0::VariantSet& variants);
    bool PathSeekToPrev(const ee0::VariantSet& variants);

//    bool UpdateNodes();

private:
    itt::WxToolbarPanel* m_toolbar = nullptr;

    std::shared_ptr<itt::SceneTree> m_stree = nullptr;

    std::shared_ptr<ee0::WxStageCanvas> m_preview_canvas = nullptr;

}; // WxGraphPage

}
}

#endif // MODULE_ITTGRAPH