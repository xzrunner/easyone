#pragma once

#include "frame/WxStageCanvas2D.h"

namespace eone
{
namespace sgraph
{

class WxStageCanvas : public WxStageCanvas2D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, const ee0::RenderContext& rc);

protected:
	virtual bool OnUpdate() override;

private:
    void OnGetVarNameChanged() const;

}; // WxStageCanvas

}
}