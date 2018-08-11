#pragma once

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

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

protected:
	virtual void DrawBackground() const;
	virtual void DrawForeground() const;

private:
	ee0::GameObj m_obj;

}; // WxStageCanvas

}
}