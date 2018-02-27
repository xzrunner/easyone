#include "frame/WxPreviewCanvas.h"

#include <ee0/WxStagePage.h>

#include <painting2/PrimitiveDraw.h>

namespace eone
{

WxPreviewCanvas::WxPreviewCanvas(ee0::WxEditPanel* stage)
	: ee0::WxStageCanvas(stage, stage->GetImpl())
{
}

void WxPreviewCanvas::OnSize(int w, int h)
{
	// 
}

void WxPreviewCanvas::OnDrawSprites() const
{
	pt2::PrimitiveDraw::Rect(nullptr, sm::vec2(0, 0), 200, 200, false);
}

}