#pragma once

#include "frame/WxStageCanvas2D.h"

namespace ur2 { class Device; }

namespace eone
{

class WxBlueprintCanvas : public WxStageCanvas2D
{
public:
	WxBlueprintCanvas(const ur2::Device& dev,
        ee0::WxStagePage* stage, const ee0::RenderContext& rc);

protected:
	virtual bool OnUpdate() override;

}; // WxBlueprintCanvas

}