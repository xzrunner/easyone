#pragma once

#include "frame/config.h"

#ifdef MODULE_MESH

#include "mesh/EditPointsMeshView.h"

#include <ee2/WxStageCanvas.h>

#include <array>

namespace draft2 { class EditShapeOP; }

namespace eone
{

class WxStagePage;

namespace mesh
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(eone::WxStagePage* stage,
		ECS_WORLD_PARAM const ee0::RenderContext& rc);

protected:
	virtual void OnDrawGUI() const override;
	virtual bool OnUpdate() override;

	virtual void DrawBackground() const override;
	virtual void DrawForeground() const override;

private:
	mutable EditPointsMeshView m_edit_view;

	mutable sm::ivec2 m_last_screen_sz;

	std::shared_ptr<draft2::EditShapeOP> m_edit_shape_op = nullptr;

	mutable uint32_t m_last_shape_type = 0;

}; // WxStageCanvas

}
}

#endif // MODULE_MESH