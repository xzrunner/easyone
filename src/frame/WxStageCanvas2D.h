#pragma once

#include <ee2/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

class WxStageCanvas2D : public ee2::WxStageCanvas
{
public:
	WxStageCanvas2D(WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

protected:
	virtual void DrawForeground() const override;

	virtual bool OnUpdate() override;

	virtual void OnMouseImpl(wxMouseEvent& event) override;

}; // WxStageCanvas2D

}