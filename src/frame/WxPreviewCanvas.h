#pragma once

#include <ee2/WxStageCanvas.h>

namespace ee2 { class WxStagePage; }

namespace eone
{

class WxPreviewCanvas : public ee2::WxStageCanvas
{
public:
	WxPreviewCanvas(ee0::WxStagePage* stage);

protected:
	virtual void DrawBackground() const;

}; // WxPreviewCanvas

}