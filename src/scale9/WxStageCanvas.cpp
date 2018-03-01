#include "scale9/WxStageCanvas.h"
#include "scale9/ComposeGrids.h"

namespace eone
{
namespace scale9
{

WxStageCanvas::WxStageCanvas(ee0::WxStagePage* stage, 
	                         const ee0::RenderContext& rc)
	: ee2::WxStageCanvas(stage, &rc)
{
}

void WxStageCanvas::DrawBackground() const
{
	ComposeGrids::Draw();
}

}
}