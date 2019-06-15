#pragma once

#include "frame/config.h"

#ifdef MODULE_MODEL

#include "frame/WxStageCanvas3D.h"

namespace eone
{
namespace model
{

class WxStageCanvas : public eone::WxStageCanvas3D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

protected:
	virtual void DrawBackground2D() const override;
	virtual void DrawForeground3D() const override;

private:
	ee0::GameObj m_obj;

}; // WxStageCanvas

}
}

#endif // MODULE_MODEL