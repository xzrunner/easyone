#pragma once

#include "frame/config.h"

#ifdef MODULE_SCALE9

#include <ee2/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

namespace scale9
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(eone::WxStagePage* stage,
		ECS_WORLD_PARAM const ee0::RenderContext& rc);

protected:
	virtual void DrawBackground() const override;
	virtual void DrawForeground() const override;

}; // WxStageCanvas

}
}

#endif // MODULE_SCALE9