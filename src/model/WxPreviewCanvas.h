#pragma once

#include "frame/config.h"

#ifdef MODULE_MODEL

#include <ee3/WxStageCanvas.h>

namespace ee0 { class SubjectMgr; }

namespace eone
{
namespace model
{

class WxPreviewPanel;

class WxPreviewCanvas : public ee3::WxStageCanvas
{
public:
	WxPreviewCanvas(WxPreviewPanel* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc, const ee0::GameObj& obj);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

protected:
    virtual void DrawForeground2D() const override;
    virtual void DrawForeground3D() const override;

	virtual bool OnUpdate() override;

private:
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

	void DrawModel() const;
	void DrawGUI() const;

private:
	ee0::GameObj m_obj;

	mutable float m_anim_speed = 1.0f;

	mutable sm::ivec2 m_last_screen_sz;

}; // WxPreviewCanvas

}
}

#endif // MODULE_MODEL