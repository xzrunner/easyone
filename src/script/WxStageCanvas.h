#pragma once

#include <ee2/WxStageCanvas.h>

#include <moon/StagePageScript.h>

namespace eone
{

class WxStagePage;

namespace script
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc, const std::string& filepath);
	virtual ~WxStageCanvas();

	virtual void OnNotify(uint32_t msg, const ee0::VariantSet& variants) override;

	void ScriptLoad();

protected:
	virtual void DrawBackground() const override;
	virtual void OnTimer() override;
	virtual void OnMouseImpl(wxMouseEvent& event) override;
	virtual void OnKeyDownImpl(wxKeyEvent& event) override;

private:
	void BindMoonCtx() const;

private:
	moon::StagePageScript m_script;

}; // WxStageCanvas

}
}