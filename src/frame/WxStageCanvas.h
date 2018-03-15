#pragma once

#include <ee2/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(WxStagePage* stage, const ee0::RenderContext& rc);

protected:
	virtual void DrawNodes() const override;

	virtual void OnMouse(int x, int y) override;

}; // WxStageCanvas

}