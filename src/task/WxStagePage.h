#pragma once

#include "frame/config.h"

#ifdef MODULE_TASK

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

#include <blueprint/WxGraphPage.h>
#include <tasklab/PreviewPage.h>

#include <taskgraph/ParamType.h>

namespace ee0 { class RenderContext; }
namespace tasklab { class WxEditorPanel; class WxToolbarPanel; }

namespace eone
{
namespace task
{

class WxStagePage : public eone::WxStagePage
{
public:
    WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj,
        const ee0::RenderContext& rc);

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
        const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

    virtual int GetPageType() const override { return PAGE_TASK; }

    //n0::SceneNodePtr GetGraphObj() const { return m_graph_obj; }

    auto& GetPreviewImpl() const { return m_preview_impl; }

    static const std::string PAGE_TYPE;

protected:
    virtual void OnPageInit() override;

    // todo ecs
#ifndef GAME_OBJ_ECS
    virtual const n0::NodeComp& GetEditedObjComp() const override;
#endif // GAME_OBJ_ECS

    virtual void StoreToJsonExt(const std::string& dir, rapidjson::Value& val,
        rapidjson::MemoryPoolAllocator<>& alloc) const override;
    virtual void LoadFromFileExt(const std::string& filepath) override;

private:
    bp::WxGraphPage<size_t>*
        CreateGraphPanel(wxWindow* parent) const;

    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);
    bool ClearSceneObj();

    void CreateNewPage(const ee0::VariantSet& variants) const;

private:
    tasklab::PreviewPage m_preview_impl;

    tasklab::WxEditorPanel*  m_editor_panel  = nullptr;
    tasklab::WxToolbarPanel* m_toolbar_panel = nullptr;

    n0::SceneNodePtr  m_graph_obj = nullptr;
    ee0::WxStagePage* m_graph_page = nullptr;

}; // WxStagePage

}
}

#endif // MODULE_TASK
