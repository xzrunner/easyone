#include "frame/WxPreviewCanvas.h"

#include <ee0/WxStagePage.h>

#include <painting2/PrimitiveDraw.h>

namespace eone
{

WxPreviewCanvas::WxPreviewCanvas(ee0::WxStagePage* stage)
	: ee2::WxStageCanvas(stage)
{
}

void WxPreviewCanvas::DrawBackground() const
{
	pt2::PrimitiveDraw::Rect(nullptr, sm::vec2(0, 0), 200, 200, false);
}

}