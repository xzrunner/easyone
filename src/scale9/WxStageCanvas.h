#pragma once

#include <ee2/WxStageCanvas.h>

namespace eone
{
namespace scale9
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(ee0::WxStagePage* stage, const ee0::RenderContext& rc,
		const ee0::WindowContext& wc);

protected:
	virtual void DrawBackground() const;

}; // WxStageCanvas

}
}