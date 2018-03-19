#pragma once

#include <ee2/WxStageCanvas.h>

#include <dust/StagePageScript.h>

namespace eone
{

class WxStagePage;

namespace script
{

class WxStageCanvas : public ee2::WxStageCanvas
{
public:
	WxStageCanvas(eone::WxStagePage* stage,
		const ee0::RenderContext& rc, const std::string& filepath);

	virtual void OnNotify(ee0::MessageID msg, const ee0::VariantSet& variants) override;

	void ScriptLoad();

protected:
	virtual void DrawBackground() const override;
	virtual void OnTimer() override;

private:
	void BindDustCtx() const;

private:
	dust::StagePageScript m_script;

}; // WxStageCanvas

}
}