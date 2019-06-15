#pragma once

#include "frame/config.h"

#ifdef MODULE_GUIGRAPH

#include <ee2/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

namespace guigraph
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
    WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);

protected:
    virtual bool OnUpdate() override;

    virtual void DrawForeground() const override;

}; // WxStageCanvas

}
}

#endif // MODULE_GUIGRAPH