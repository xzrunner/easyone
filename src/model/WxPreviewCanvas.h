#pragma once

#include <ee3/WxStageCanvas.h>

namespace ur { class Shader; }
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
	virtual void DrawBackground() const override;
	virtual void DrawForeground() const override;

private:
	void RegisterMsg(ee0::SubjectMgr& sub_mgr);

private:
	ee0::GameObj m_obj;

}; // WxPreviewCanvas

}
}