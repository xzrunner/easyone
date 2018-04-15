#pragma once

#include <ee2/WxStageCanvas.h>

namespace eone
{

class WxStagePage;

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(
		WxStagePage* stage, 
#ifdef GAME_OBJ_ECS
		ecsx::World& world,
#endif // GAME_OBJ_ECS
		const ee0::RenderContext& rc
	);

protected:
	virtual void DrawNodes() const override;

	virtual bool OnUpdate() override;

	virtual void OnMouseImpl(wxMouseEvent& event) override;

}; // WxStageCanvas

}