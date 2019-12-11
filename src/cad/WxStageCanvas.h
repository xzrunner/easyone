#pragma once

#include "frame/config.h"

#ifdef MODULE_CAD

#include <ee2/WxStageCanvas.h>

#include <array>

namespace sketch { class EditShapeOP; }
namespace dw2 { class EditView; }

namespace eone
{

class WxStagePage;

namespace cad
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(eone::WxStagePage* stage,
		ECS_WORLD_PARAM const ee0::RenderContext& rc,
        draft2::EditView& edit_view);

protected:
	virtual void OnDrawGUI() const override;
	virtual bool OnUpdate() override;

	virtual void DrawBackground() const override;

private:
	mutable sm::ivec2 m_last_screen_sz;

	std::shared_ptr<sketch::EditShapeOP> m_edit_shape_op = nullptr;

	mutable uint32_t m_last_shape_type = 0;

}; // WxStageCanvas

}
}

#endif // MODULE_CAD