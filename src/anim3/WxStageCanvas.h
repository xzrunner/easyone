#pragma once

#include "frame/WxStageCanvas3D.h"

namespace eone
{
namespace anim3
{

class WxStageCanvas : public eone::WxStageCanvas3D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

protected:
	virtual void DrawForeground3D() const override;

private:
	ee0::GameObj m_obj;

}; // WxStageCanvas

}
}