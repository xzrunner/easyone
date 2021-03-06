#pragma once

#include "frame/config.h"

#ifdef MODULE_TOUCH_DESIGNER

#include "frame/WxStagePage.h"
#include "frame/StagePageType.h"

#include <tdv/PreviewPage.h>

namespace tdv { class WxEditorPanel; class WxToolbarPanel; class WxGraphPage; }

namespace eone
{
namespace touchdesigner
{

class WxStagePage : public eone::WxStagePage
{
public:
    WxStagePage(wxWindow* parent, ECS_WORLD_PARAM const ee0::GameObj& obj);

    virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

    virtual void Traverse(std::function<bool(const ee0::GameObj&)> func,
        const ee0::VariantSet& variants = ee0::VariantSet(), bool inverse = false) const override;

    virtual int GetPageType() const override { return PAGE_TOUCH_DESIGNER; }

    //n0::SceneNodePtr GetGraphObj() const { return m_graph_obj; }

    void InitEditOP() { m_preview_impl.InitEditOP(); }

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
    tdv::WxGraphPage* CreateGraphPanel(wxWindow* parent) const;

    bool InsertSceneObj(const ee0::VariantSet& variants);
    bool DeleteSceneObj(const ee0::VariantSet& variants);
    bool ClearSceneObj();

    void CreateNewPage(const ee0::VariantSet& variants) const;

private:
    tdv::PreviewPage m_preview_impl;

    tdv::WxEditorPanel*  m_editor_panel  = nullptr;
    tdv::WxToolbarPanel* m_toolbar_panel = nullptr;

    n0::SceneNodePtr m_graph_obj = nullptr;
    tdv::WxGraphPage* m_graph_page = nullptr;

}; // WxStagePage

}
}

#endif // MODULE_TOUCH_DESIGNER