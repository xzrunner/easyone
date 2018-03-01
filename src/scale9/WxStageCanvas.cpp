#include "scale9/WxStageCanvas.h"
#include "scale9/ComposeGrids.h"

namespace eone
{
namespace scale9
{

WxStageCanvas::WxStageCanvas(ee0::WxStagePage* stage, 
	                         const ee0::RenderContext& rc, 
	                         const ee0::WindowContext& wc)
	: ee2::WxStageCanvas(stage, &rc, &wc)
{
}

void WxStageCanvas::DrawBackground() const
{
	ComposeGrids::Draw();
}

}
}