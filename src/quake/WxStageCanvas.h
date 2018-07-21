#pragma once

#include <frame/WxStageCanvas3D.h>

namespace ur { class Shader; }

namespace eone
{

class WxStagePage;

namespace quake
{

class WxStageCanvas : public WxStageCanvas3D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

protected:
	virtual void DrawBackground() const override;
	virtual void DrawForeground() const override;

private:
	void InitShaders() const;

private:
	mutable std::shared_ptr<ur::Shader> m_face_shader;
	mutable std::shared_ptr<ur::Shader> m_edge_shader;

}; // WxStageCanvas

}
}