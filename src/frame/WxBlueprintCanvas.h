#pragma once

#include "frame/WxStageCanvas2D.h"

namespace eone
{

class WxBlueprintCanvas : public WxStageCanvas2D
{
public:
	WxBlueprintCanvas(eone::WxStagePage* stage, const ee0::RenderContext& rc);

protected:
	virtual bool OnUpdate() override;

}; // WxBlueprintCanvas

}