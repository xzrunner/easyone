#include "quake/WxStageCanvas.h"

namespace eone
{
namespace quake
{

WxStageCanvas::WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
	                         const ee0::RenderContext& rc)
	: WxStageCanvas3D(stage, ECS_WORLD_VAR rc)
{
}

void WxStageCanvas::OnDrawSprites() const
{
	WxStageCanvas3D::OnDrawSprites();
}

}
}