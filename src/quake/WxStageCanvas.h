#pragma once

#include <frame/WxStageCanvas3D.h>

namespace eone
{

class WxStagePage;

namespace quake
{

class WxStageCanvas : public WxStageCanvas3D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

protected:
	virtual void DrawBackground() const override;
	virtual void DrawForeground() const override;

private:
	void InitShaders() const;

}; // WxStageCanvas

}
}