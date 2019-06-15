#pragma once

#include "frame/config.h"
#include "frame/WxStageCanvas2D.h"

#ifdef MODULE_PHYSICS3D

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

namespace eone
{
namespace physics3d
{

class WxStageCanvas2D : public eone::WxStageCanvas2D
{
public:
    WxStageCanvas2D(eone::WxStagePage* stage, const ee0::RenderContext& rc);

protected:
    virtual bool OnUpdate() override;

}; // WxStageCanvas2D

}
}

#endif // MODULE_PHYSICS3D