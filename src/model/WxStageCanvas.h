#pragma once

#include "frame/WxStageCanvas3D.h"

namespace model { class ModelInstance; }

namespace eone
{
namespace model
{

class WxStageCanvas : public eone::WxStageCanvas3D
{
public:
	WxStageCanvas(eone::WxStagePage* stage, ECS_WORLD_PARAM
		const ee0::RenderContext& rc);

protected:
	virtual void DrawBackground() const;
	virtual void DrawForeground() const;

private:
	void InitShaders() const;

	void DrawImpl(const ::model::ModelInstance& model_inst,
		const n3::RenderParams& params) const;

	void DrawSkeletalNode(const ::model::ModelInstance& model_inst,
		int node_idx, const n3::RenderParams& params) const;

private:
	ee0::GameObj m_obj;

}; // WxStageCanvas

}
}