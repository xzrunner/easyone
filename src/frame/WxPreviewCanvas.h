#pragma once

#include <ee0/WxStageCanvas.h>

namespace ee0 { class WxEditPanel; }

namespace eone
{

class WxPreviewCanvas : public ee0::WxStageCanvas
{
public:
	WxPreviewCanvas(ee0::WxEditPanel* stage);

protected:
	virtual void OnSize(int w, int h) override;
	virtual void OnDrawSprites() const override;

}; // WxPreviewCanvas

}