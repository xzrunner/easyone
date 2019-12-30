#pragma once

#include "frame/config.h"

#ifdef MODULE_SHAPE3D

#include <frame/WxStageCanvas3D.h>

namespace draft3 { class EditOpMgr; }

namespace eone
{

class WxStagePage;

namespace shape3d
{

class WxStageCanvas : public WxStageCanvas3D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc, draft3::EditOpMgr& op_mgr);

protected:
    virtual void DrawBackground2D() const override;
    virtual void DrawForeground2D() const override;
    virtual void DrawForeground3D() const override;

private:
	void InitShaders() const;

	void DrawSceneNodes() const;
	void DrawGUI() const;

private:
	draft3::EditOpMgr& m_op_mgr;

}; // WxStageCanvas

}
}

#endif // MODULE_SHAPE3D