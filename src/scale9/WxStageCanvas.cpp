#include "scale9/WxStageCanvas.h"
#include "scale9/ComposeGrids.h"

namespace eone
{
namespace scale9
{

WxStageCanvas::WxStageCanvas(ee0::WxStagePage* stage, std::shared_ptr<wxGLContext> glctx)
	: ee2::WxStageCanvas(stage, glctx)
{
}

void WxStageCanvas::DrawBackground() const
{
	ComposeGrids::Draw();
}

}
}