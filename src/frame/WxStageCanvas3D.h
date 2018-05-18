#pragma once

#include <ee3/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

class WxStageCanvas3D : public ee3::WxStageCanvas
{
public:
	WxStageCanvas3D(WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

protected:
	virtual bool OnUpdate() override;

}; // WxStageCanvas3D

}