#pragma once

#include <ee2/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

namespace scale9
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(eone::WxStagePage* stage, const ee0::RenderContext& rc);

protected:
	virtual void DrawBackground() const override;
	virtual void DrawNodes() const override;

}; // WxStageCanvas

}
}