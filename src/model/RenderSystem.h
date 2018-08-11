#pragma once

#include <cu/cu_macro.h>

#include <memory>

namespace ur { class Shader; }
namespace n3 { class RenderParams; }
namespace model { class ModelInstance; }

namespace eone
{
namespace model
{

class RenderSystem
{
public:
	void DrawModel(const ::model::ModelInstance& model_inst,
		const n3::RenderParams& params, bool polygon_line = false) const;

private:
	void DrawSkeletalNode(const ::model::ModelInstance& model_inst,
		int node_idx, const n3::RenderParams& params) const;

private:
	void InitShaders();

private:
	std::shared_ptr<ur::Shader> m_default_shader = nullptr;
	std::shared_ptr<ur::Shader> m_shape_shader   = nullptr;

	CU_SINGLETON_DECLARATION(RenderSystem);

}; // RenderSystem

}
}