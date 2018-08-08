#pragma once

#include <ee0/WxStageCanvas.h>
#include <ee0/Observer.h>
#include <ee0/Config.h>

namespace ee0 { class SubjectMgr; }
namespace pt0 { class Camera; }
ECS_WORLD_DECL

namespace eone
{

class WxPreviewPanel;

class WxPreviewCanvas : public ee0::WxStageCanvas, public ee0::Observer
{
public:
	WxPreviewCanvas(WxPreviewPanel* stage, ECS_WORLD_PARAM const ee0::RenderContext& rc);

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

protected:
	virtual void OnSize(int w, int h) override;
	virtual void OnDrawSprites() const override;

private:
	WxPreviewPanel* m_stage;
	ECS_WORLD_SELF_DEF

}; // WxPreviewCanvas

}