#pragma once

#include <ee3/WxStageCanvas.h>

namespace ur { class Shader; }
namespace ee0 { class SubjectMgr; }

namespace eone
{
namespace quake
{

class WxPreviewPanel;

class WxPreviewCanvas : public ee3::WxStageCanvas
{
public:
	WxPreviewCanvas(WxPreviewPanel* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

protected:
    virtual void DrawForeground3D() const override;

private:
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

private:
	mutable n0::SceneNodePtr m_node = nullptr;

}; // WxPreviewCanvas

}
}