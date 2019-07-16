#pragma once

#include "frame/config.h"

#ifdef MODULE_ITTGRAPH

#include <ee3/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

namespace ittgraph
{

class WxStageCanvas : public ee3::WxStageCanvas
{
public:
    WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
        const ee0::RenderContext& rc);

protected:
    virtual bool OnUpdate() override;

    virtual void DrawForeground3D() const override;

}; // WxStageCanvas

}
}

#endif // MODULE_ITTGRAPH