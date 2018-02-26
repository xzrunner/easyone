#pragma once

#include <ee2/WxStageCanvas.h>

namespace eone
{
namespace scale9
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(ee0::WxStagePage* stage, std::shared_ptr<wxGLContext> glctx);

protected:
	virtual void DrawBackground() const;

}; // WxStageCanvas

}
}