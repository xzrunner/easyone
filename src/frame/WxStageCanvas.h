#pragma once

#include <ee2/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

protected:
	virtual void DrawNodes() const override;

	virtual bool OnUpdate() override;

	virtual void OnMouseImpl(wxMouseEvent& event) override;

}; // WxStageCanvas

}