#pragma once

#include "frame/WxStageCanvas3D.h"

namespace eone
{
namespace physics3d
{

class WxStageCanvas : public WxStageCanvas3D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, const ee0::RenderContext& rc);

protected:
	virtual bool OnUpdate() override;

private:
    clock_t m_last_time = 0;

}; // WxStageCanvas

}
}