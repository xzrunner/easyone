#pragma once

#include <frame/WxStageCanvas3D.h>

namespace dw3 { class EditOpMgr; }

namespace eone
{

class WxStagePage;

namespace shape3d
{

class WxStageCanvas : public WxStageCanvas3D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc, dw3::EditOpMgr& op_mgr);

protected:
	virtual void DrawBackground() const override;
	virtual void DrawForeground() const override;

private:
	void InitShaders() const;

	void DrawSceneNodes() const;
	void DrawGUI() const;

private:
	dw3::EditOpMgr& m_op_mgr;

}; // WxStageCanvas

}
}